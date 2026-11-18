#pragma once
#include "Recording.h"
#include "FrequencyScale.h"

namespace spectrumtag {
// A fast expected-power preview, not an audio render. Always start from the
// immutable original spectrum: reducing Fill must remove the previous noise.
// STFT phase, overlap/window leakage and stereo correlation are approximated;
// the exact rendered spectrum replaces this image after the gesture finishes.
inline juce::Image printPreview(const RecordingSnapshot& source,const PrintEdit& edit,int palette,bool fillEmpty=false,
        const std::vector<PrintEdit>* remainingEdits=nullptr,juce::Point<int> pixelSize={},const RecordingSnapshot* savedBackground=nullptr,const SpectrumHistory* live=nullptr) {
    if((!live&&source.empty())||(!remainingEdits&&!edit.valid())||edit.duration<=0||edit.low>=edit.high)return {};
    const int w=pixelSize.x>0?pixelSize.x:juce::jmin(512,edit.cols);
    const int h=pixelSize.y>0?pixelSize.y:juce::jmin(512,edit.rows);
    const float nyquist=(float)source.sampleRate*.5f;
    std::vector<double> times((size_t)w);
    std::vector<float> frequencies((size_t)h);
    std::vector<const SpectrumFrame*> columns((size_t)w);
    std::vector<bool> exactBackground((size_t)w,false);
    for(int x=0;x<w;++x){times[x]=edit.start+(x+.5)*edit.duration/w;
        if(live){
            const auto& frames=live->frames;
            const auto it=std::lower_bound(frames.begin(),frames.end(),times[x],[](const auto& f,double time){return f->time<time;});
            if(it!=frames.end()&&(*it)->time-times[x]<=(*it)->interval*1.5)columns[x]=it->get();
        }else if(savedBackground&&times[x]>=savedBackground->start()&&times[x]<savedBackground->end()){
            columns[x]=savedBackground->frameAt(times[x]);
            exactBackground[x]=true;
        }else if(remainingEdits||source.contains(llround(times[x]*source.sampleRate)))columns[x]=source.frameAt(times[x],false);
    }
    for(int y=0;y<h;++y)frequencies[y]=frequency::interpolate(1.f-(float)y/juce::jmax(1,h-1),edit.low*nyquist,edit.high*nyquist,edit.scaleMode)/nyquist;
    struct Layer {
        const PrintEdit* edit=nullptr;float floorPower=0;
        std::vector<int> rows,cols,earlierCols,laterCols;
        std::vector<float> edges,envelopes;
    };
    std::vector<Layer> layers;
    auto add=[&](const PrintEdit& e){
        if(!e.valid()||e.start>=edit.start+edit.duration||e.start+e.duration<=edit.start||e.low>=edit.high||e.high<=edit.low)return;
        Layer layer;layer.edit=&e;
        layer.floorPower=e.compensation>0?std::pow(10.f,(-66.f+42.f*e.compensation)/10.f):0;
        // Any mask can contain detail narrower than a single STFT
        // window. Sample its neighbouring time slices to estimate the
        // overlap-add response instead of displaying razor-sharp transitions.
        const bool smoothMask=remainingEdits==nullptr||fillEmpty;
        if(smoothMask){layer.earlierCols.resize(w);layer.laterCols.resize(w);}
        layer.rows.resize(h,-1);layer.cols.resize(w,-1);layer.edges.resize(h);layer.envelopes.resize(w);
        for(int y=0;y<h;++y){const float f=frequencies[y];if(f<e.low||f>e.high)continue;
            layer.rows[y]=juce::jlimit(0,e.rows-1,juce::roundToInt((f-e.low)/(e.high-e.low)*(e.rows-1)));
            const float edge=juce::jlimit(0.f,1.f,juce::jmin(f-e.low,e.high-f)*e.fftSize*.25f);
            layer.edges[y]=edge*edge;
        }
        const int64_t start=llround(e.start*source.sampleRate),end=llround((e.start+e.duration)*source.sampleRate);
        const double fade=juce::jmax(1.,juce::jmin(source.sampleRate*.005,(end-start)*.1));
        for(int x=0;x<w;++x){const auto at=llround(times[x]*source.sampleRate);if(at<start||at>=end)continue;
            auto maskCol=[&](double time){return juce::jlimit(0,e.cols-1,(int)((time-e.start)/e.duration*e.cols));};
            const double centre=smoothMask&&columns[x]?columns[x]->time:times[x];
            layer.cols[x]=maskCol(centre);
            if(smoothMask){
                const double step=.22*e.fftSize/source.sampleRate;
                layer.earlierCols[x]=maskCol(centre-step);layer.laterCols[x]=maskCol(centre+step);
            }
            const float envelope=std::sin(juce::jlimit(0.f,1.f,(float)(juce::jmin(at-start,end-1-at)/fade))*juce::MathConstants<float>::halfPi);
            layer.envelopes[x]=envelope*envelope;
        }
        layers.push_back(std::move(layer));
    };
    bool replaced=false;
    if(remainingEdits){
        for(const auto& existing:*remainingEdits)add(existing);
    }else{
        for(const auto& existing:source.edits){if(existing.id==edit.id){add(edit);replaced=true;}else add(existing);}
        if(!replaced)add(edit);
    }
    // LUTs keep palette conversion and exponentials out of the pixel loop.
    static const auto powers=[] {std::array<float,256> values{};for(int i=1;i<256;++i)values[i]=std::pow(10.f,(i*96.f/255.f-96.f)/10.f);return values;}();
    std::array<juce::PixelRGB,256> colours;
    for(int i=0;i<256;++i){auto c=spectrumColour(palette,i/255.f);colours[i].setARGB(255,c.getRed(),c.getGreen(),c.getBlue());}
    // macOS native RGB images are actually ARGB. Keep this CPU-side preview in
    // packed software RGB so PixelRGB stores cannot corrupt alpha or neighbours.
    juce::Image image(juce::Image::RGB,w,h,false,juce::SoftwareImageType{});juce::Image::BitmapData pixels(image,juce::Image::BitmapData::writeOnly);
    for(int y=0;y<h;++y){
        auto* row=reinterpret_cast<juce::PixelRGB*>(pixels.getLinePointer(y));
        for(int x=0;x<w;++x){const auto* frame=columns[x];float energy=0;
            if(frame&&frame->bins>1){
                const float bin=juce::jlimit(0.f,(float)frame->bins-1,frequencies[y]*(frame->bins-1));
                const int lo=(int)bin,hi=juce::jmin(frame->bins-1,lo+1);
                const int level=juce::jlimit(0,255,juce::roundToInt(frame->level[lo]+(bin-lo)*(frame->level[hi]-frame->level[lo])));
                energy=powers[level];
            }
            if(!exactBackground[x]&&(fillEmpty||(frame&&frame->bins>1))){
                for(const auto& layer:layers){const int r=layer.rows[y],c=layer.cols[x];if(r<0||c<0)continue;
                    const auto& e=*layer.edit;const float* maskRow=e.mask->data()+(size_t)r*e.cols;
                    float mask=maskRow[c];
                    if(!layer.earlierCols.empty())
                        mask=.5f*mask+.25f*(maskRow[layer.earlierCols[x]]+maskRow[layer.laterCols[x]]);
                    const float affected=e.invert?1-mask:mask;
                    const float gain=1-affected+affected*e.ratio;
                    // Display frames average channel power, not L+R samples.
                    const float fill=juce::jmax(0.f,layer.floorPower-energy);
                    energy=energy*gain*gain+fill*(1-affected)*(1-affected)*layer.edges[y]*layer.envelopes[x];
                }
            }
            const int level=energy>0?juce::jlimit(0,255,juce::roundToInt((10.f*std::log10(energy)+96.f)*255.f/96.f)):0;
            row[x]=colours[level];
        }
    }
    return image;
}
}
