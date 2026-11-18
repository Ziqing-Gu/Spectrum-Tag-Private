#pragma once
#include "shared/PrintPreview.h"
// Shared editing contract exercised with a standalone and a recording plug-in.
static void revision204Checks(const juce::File& dir,bool filePlayer=true){
    using namespace spectrumtag;
    SpectrumTagAudioProcessor p;p.filePlayerMode=filePlayer;
    p.setRateAndBufferSizeDetails(48000,256);p.prepareToPlay(48000,256);
    juce::AudioBuffer<float> source(1,96000);source.clear();
    p.recording().setArmed(true);p.recording().capture(source,0,48000,true);p.recording().setArmed(false);
    require(p.recording().flush(),"204 silent imported source");
    std::unique_ptr<juce::AudioProcessorEditor> editor(p.createEditor());
    auto pump=[] {juce::Thread::sleep(12);juce::Timer::callPendingTimersSynchronously();};
    auto wait=[&](auto done,const char* why){const auto stop=juce::Time::getMillisecondCounter()+12000;while(!done()&&juce::Time::getMillisecondCounter()<stop)pump();require(done(),why);};
    auto* view=componentWithID(*editor,"spectrumView");auto* box=imageBox(*editor);
    require(view&&box,"204 components");
    wait([&]{return (bool)view->getProperties()["recordMode"];},"204 recording view");
    fitSpectrum(*view);pump();
    juce::Image art(juce::Image::RGB,160,120,false);
    for(int y=0;y<120;++y)for(int x=0;x<160;++x)art.setPixelAt(x,y,juce::Colour::fromFloatRGBA(x/159.f,y/119.f,.25f,1.f));
    const auto file=dir.getChildFile("204-gradient.png");png(file,art);
    dynamic_cast<juce::FileDragAndDropTarget*>(editor.get())->filesDropped({file.getFullPathName()},0,0);
    wait([&]{return box->conversionReady();},"204 initial conversion");
    box->setNormalisedBounds({.2f,.15f,.3f,.5f});if(box->onChanged)box->onChanged();
    auto slider=[&](const char* id){auto* s=dynamic_cast<juce::Slider*>(componentWithID(*editor,id));require(s!=nullptr,"204 slider exists");return s;};
    auto* fill=slider("noiseFill");auto* brightness=slider("imageGrayBrightness");auto* contrast=slider("imageGrayContrast");
    auto lit=[](const juce::Image& im){double v=0;for(int y=0;y<im.getHeight();++y)for(int x=0;x<im.getWidth();++x)v+=im.getPixelAt(x,y).getBrightness();return v/juce::jmax(1,im.getWidth()*im.getHeight());};
    auto diff=[](const juce::Image& a,const juce::Image& b,juce::Rectangle<int> area){int n=0;for(int y=area.getY();y<area.getBottom();++y)for(int x=area.getX();x<area.getRight();++x)n+=std::abs(a.getPixelAt(x,y).getBrightness()-b.getPixelAt(x,y).getBrightness())>.035f;return n;};
    auto* style=dynamic_cast<juce::ComboBox*>(componentWithID(*editor,"imageStyle"));
    auto chromatic=[](const juce::Image& im,juce::Rectangle<int> r){
        int count=0;
        for(int y=r.getY();y<r.getBottom();++y)for(int x=r.getX();x<r.getRight();++x){
            auto c=im.getPixelAt(x,y);
            if(juce::jmax(c.getRed(),c.getGreen(),c.getBlue())-juce::jmin(c.getRed(),c.getGreen(),c.getBlue())>12)++count;
        }
        return count;
    };
    auto levelCount=[](const juce::Image& im){
        std::array<bool,256> used{};
        for(int y=0;y<im.getHeight();++y)for(int x=0;x<im.getWidth();++x)used[im.getPixelAt(x,y).getRed()]=true;
        return std::count(used.begin(),used.end(),true);
    };
    for(int i=0;i<5;++i)pump();
    require(!box->spectrumPreview.isValid(),"206 first load shows processed grayscale source");
    for(auto styleId:{1,2}){
        fill->setValue(styleId==1?.4:.6,juce::sendNotificationSync);
        wait([&]{return box->spectrumPreview.isValid();},"208 parameter starts live simulation");
        style->setSelectedId(styleId,juce::sendNotificationSync);
        require(!box->spectrumPreview.isValid(),"208 switching clears the old simulation immediately");
        wait([&]{return box->conversionReady();},"208 new style conversion");
        for(int i=0;i<20;++i)pump();
        const auto sourceView=box->createComponentSnapshot(box->getLocalBounds());
        const auto inner=sourceView.getBounds().reduced(5).withTrimmedBottom(25);
        require(!box->spectrumPreview.isValid()&&chromatic(sourceView,inner)==0,"208 first view after each switch stays monochrome");
        const auto levels=levelCount(box->getPreviewImage());
        require(styleId==1?levels==2:levels>32,"208 Logo has two source levels and Complex has continuous grayscale");
    }
    fill->onDragStart();fill->setValue(.1,juce::sendNotificationSync);fill->setValue(0,juce::sendNotificationSync);pump();
    for(int i=0;i<6;++i)pump();
    auto blank=box->createComponentSnapshot(box->getLocalBounds());
    fill->setValue(.8,juce::sendNotificationSync);for(int i=0;i<6;++i)pump();auto filled=box->createComponentSnapshot(box->getLocalBounds());
    std::cout<<"Placing Noise Fill brightness "<<lit(blank)<<" -> "<<lit(filled)<<"\n";
    require(lit(filled)>lit(blank)+.08,"204 placing Noise Fill previews on silence during drag");
    require(p.recording().edits()->empty(),"204 placing preview does not print audio");fill->onDragEnd();
    require(!box->spectrumPreview.isValid()&&box->isVisible(),"209 releasing a slider immediately restores source artwork");
    const auto sourceInView=view->getLocalPoint(box,box->getLocalBounds().getCentre()).toFloat();
    view->mouseDown(mouseAt(*view,sourceInView,true));view->mouseUp(mouseAt(*view,sourceInView,true));pump();
    require(!box->isVisible()&&!style->isEnabled()&&p.getEditorState().retainHiddenParameters,"209 right-click hides the source even while it has edits");
    const juce::Point<float> blankPoint{82,4};
    view->mouseDown(mouseAt(*view,blankPoint));view->mouseUp(mouseAt(*view,blankPoint));
    view->mouseDown(mouseAt(*view,blankPoint,true));view->mouseUp(mouseAt(*view,blankPoint,true));pump();
    const auto centreAfterShow=box->getNormalisedBounds().getCentre();
    require(box->isVisible()&&!box->spectrumPreview.isValid()&&std::abs(fill->getValue()-.8)<.001
        &&centreAfterShow.getDistanceFrom({.5f,.5f})<.003f,"209 right-click blank shows current artwork at plot centre with retained parameters");
    fill->setValue(.79,juce::sendNotificationSync);
    wait([&]{return box->spectrumPreview.isValid();},"208 next control adjustment resumes new-placement simulation");
    fill->setValue(.8,juce::sendNotificationSync);
    wait([&]{return box->conversionReady();},"204 preview ready");
    auto* print=dynamic_cast<juce::Button*>(componentWithID(*editor,"printButton"));require(print!=nullptr,"204 print control");print->onClick();
    require(box->isVisible()&&!box->spectrumPreview.isValid(),"209 image stays visible during its explicit Print");
    wait([&]{auto s=p.recording().snapshot();return s&&s->edits.size()==1&&!p.recording().isBusy()&&!(bool)view->getProperties()["previewActive"];},"204 first Print completes");
    auto* approve=dynamic_cast<juce::Button*>(componentWithID(*editor,"imagePrint"));
    auto* cancel=dynamic_cast<juce::Button*>(componentWithID(*editor,"cancelPrint"));
    require(approve&&cancel,"205 Print and Cancel controls exist");
    const auto id=p.recording().edits()->front().id;require(!box->isVisible(),"204 Print hides source image");
    require(p.recording().edits()->front().imageStyle==1,"204 retains complex mode");
    auto shot=[&]{return view->createComponentSnapshot(view->getLocalBounds());};
    auto area=[&]{shot();auto& v=view->getProperties();return juce::Rectangle<int>((int)v["selectedX"],(int)v["selectedY"],(int)v["selectedW"],(int)v["selectedH"]).reduced(5).withTrimmedTop(20).getIntersection(view->getLocalBounds());};
    auto bounds=area();require(bounds.getWidth()>20&&bounds.getHeight()>20,"204 selected footprint visible");
    auto drag=[&](juce::Slider* s,double lo,double hi,const char* why){
        const auto audio=p.recording().snapshot();const auto passes=p.recording().printPasses();
        s->onDragStart();s->setValue(lo,juce::sendNotificationSync);wait([&]{return box->conversionReady();},"204 low conversion");for(int i=0;i<5;++i)pump();const auto low=shot();
        s->setValue(hi,juce::sendNotificationSync);wait([&]{return box->conversionReady();},"204 high conversion");for(int i=0;i<5;++i)pump();const auto high=shot();
        require(diff(low,high,bounds)>40,why);
        require(p.recording().snapshot()==audio&&p.recording().printPasses()==passes,"204 no audio rendering while held");
        require(p.recording().edits()->size()==1&&p.recording().edits()->front().id==id,"204 edit retained during gesture");
        png(dir.getChildFile(juce::String("204-drag-")+s->getComponentID()+".png"),high);
        s->onDragEnd();
        for(int i=0;i<25;++i)pump();
        require(approve->isVisible()&&p.recording().snapshot()==audio&&p.recording().printPasses()==passes,"205 no audio job after releasing a parameter");
        approve->onClick();wait([&]{return !p.recording().isBusy()&&p.recording().snapshot()!=audio&&!(bool)view->getProperties()["previewActive"];},"204 release replaces audio and retires preview");
        require(p.recording().edits()->size()==1&&p.recording().edits()->front().id==id,"204 release replaces same Print");
        std::cout<<"PASS "<<why<<"\n"<<std::flush;
    };
    drag(brightness,.2,.8,"204 complex brightness follows held drag in the printed spectrum");
    drag(contrast,.1,.9,"204 complex contrast follows held drag in the printed spectrum");
    drag(fill,0,.6,"204 Noise Fill adds and removes spectrum preview while held");
    drag(slider("amplitudeRatio"),0,.3,"204 ratio follows held drag");
    auto* method=dynamic_cast<juce::ComboBox*>(componentWithID(*editor,"imageMode"));
    const auto savedAudio=p.recording().snapshot();const auto savedPrint=p.recording().edits()->front();
    style->setSelectedId(1,juce::sendNotificationSync);
    for(int i=0;i<24;++i)pump();
    require(!box->isVisible()&&approve->isVisible()&&chromatic(shot(),bounds)>20,"209 hidden Print shows spectral prediction when switching style");
    auto rightOnPrint=[&]{const auto pt=bounds.getCentre().toFloat();view->mouseDown(mouseAt(*view,pt,true));view->mouseUp(mouseAt(*view,pt,true));pump();};
    auto* amount208=slider("imageAmount");
    amount208->onDragStart();amount208->setValue(.31,juce::sendNotificationSync);
    wait([&]{return chromatic(shot(),bounds)>20;},"208 adjusting a hidden Print shows spectrum during gesture");
    rightOnPrint();
    require(box->isVisible()&&approve->isVisible()&&!box->spectrumPreview.isValid(),"209 right-click shows the working source in image mode");
    amount208->onDragEnd();amount208->onDragStart();amount208->setValue(.39,juce::sendNotificationSync);
    wait([&]{return box->spectrumPreview.isValid();},"209 moving an image-mode slider predicts the result");
    amount208->onDragEnd();
    require(!box->spectrumPreview.isValid(),"209 releasing image-mode slider restores black and white immediately");
    for(int i=0;i<24;++i)pump();
    require(!box->spectrumPreview.isValid()&&chromatic(shot(),bounds)==0,"209 settled image remains monochrome after background conversion");
    rightOnPrint();
    require(!box->isVisible()&&p.getEditorState().standaloneDraft&&chromatic(shot(),bounds)>20
        &&std::abs(p.getEditorState().standaloneDraft->start-savedPrint.start)<1.e-6
        &&std::abs(amount208->getValue()-.39)<.001&&p.recording().snapshot()==savedAudio,"209 right-click hides the source and resumes the same pending Print");
    amount208->onDragStart();amount208->setValue(.45,juce::sendNotificationSync);
    wait([&]{return chromatic(shot(),bounds)>20;},"209 parameters keep predicting while the image is hidden");amount208->onDragEnd();
    for(int i=0;i<24;++i)pump();
    require(!box->isVisible()&&chromatic(shot(),bounds)>20&&p.recording().snapshot()==savedAudio,"209 release in Print mode keeps the spectrum prediction");
    method->setSelectedId(3,juce::sendNotificationSync);
    for(int i=0;i<20;++i)pump();
    require(p.recording().edits()->front().imageStyle==1,"205 combo edits remain drafts");approve->onClick();
    wait([&]{auto s=p.recording().snapshot();return s&&s->edits[0].imageStyle==0&&s->edits[0].imageMode==2&&!p.recording().isBusy()&&!(bool)view->getProperties()["previewActive"];},"205 manual Print applies Logo mode");
    drag(slider("imageAmount"),.2,.8,"204 Logo threshold follows held drag");
    auto* speed=slider("speed");require(speed!=nullptr,"204 Speed control");
    const auto beforeSpeed=p.recording().edits()->front();const auto speedAudio=p.recording().snapshot();
    speed->onDragStart();speed->setValue(beforeSpeed.speed*1.5,juce::sendNotificationSync);wait([&]{return (bool)view->getProperties()["previewActive"];},"204 Speed publishes intermediate preview");shot();
    require(p.recording().snapshot()==speedAudio&&(bool)view->getProperties()["previewActive"],"204 Speed shows preview without audio work while held");
    speed->onDragEnd();
    for(int i=0;i<20;++i)pump();
    require(p.recording().snapshot()==speedAudio,"205 Speed release keeps audio");approve->onClick();wait([&]{return p.recording().snapshot()!=speedAudio&&!p.recording().isBusy()&&!(bool)view->getProperties()["previewActive"];},"204 Speed commits on release");
    require(std::abs(p.recording().edits()->front().duration-beforeSpeed.duration/1.5)<.0001,"204 Speed updates duration");
    const auto committedSpeed=p.recording().edits()->front();
    editor->keyPressed(juce::KeyPress('z',juce::ModifierKeys::ctrlModifier,0));
    wait([&]{return sameEdit(p.recording().edits()->front(),beforeSpeed)&&!p.recording().isBusy();},"205 undo explicit Print");
    editor->keyPressed(juce::KeyPress('z',juce::ModifierKeys::ctrlModifier|juce::ModifierKeys::shiftModifier,0));
    wait([&]{return sameEdit(p.recording().edits()->front(),committedSpeed)&&!p.recording().isBusy();},"205 redo explicit Print");
    for(const bool resize:{false,true}){
        (void)shot();
        const auto before=p.recording().edits()->front();
        const auto audio=p.recording().snapshot();const auto passes=p.recording().printPasses();
        auto& v=view->getProperties();
        juce::Rectangle<float> rect((float)v["selectedX"],(float)v["selectedY"],(float)v["selectedW"],(float)v["selectedH"]);
        auto point=resize?rect.getBottomRight().translated(-4,-4):rect.getCentre();
        auto target=point.translated(24,resize?15.f:3.f);
        view->mouseDown(mouseAt(*view,point));view->mouseDrag(mouseAt(*view,target));
        require((bool)v["previewActive"],"204 moving/resizing publishes a preview during drag");
        view->mouseUp(mouseAt(*view,target));
        for(int i=0;i<20;++i)pump();
        require(approve->isVisible()&&p.recording().snapshot()==audio&&sameEdit(before,p.recording().edits()->front())&&p.recording().printPasses()==passes,"204 releasing geometry only shows Print");
        approve->onClick();
        wait([&]{return p.recording().snapshot()!=audio&&!p.recording().isBusy()&&!(bool)v["previewActive"];},"204 clicking Print commits pending geometry");
        auto after=p.recording().edits()->front();
        require(after.id==id&&p.recording().edits()->size()==1&&(resize?after.duration!=before.duration:after.start!=before.start),"204 confirmed geometry replaces original");
    }
    {
        const auto audio=p.recording().snapshot();const auto edit=p.recording().edits()->front();
        const auto savedSpeed=speed->getValue();
        fitSpectrum(*view);pump();
        const float y=(float)view->getHeight()-55;
        const auto edge=juce::Point<float>((float)view->getWidth()-2,y);
        const auto target=edge.translated(-110,0);
        view->mouseDown(mouseAt(*view,edge));view->mouseDrag(mouseAt(*view,target));view->mouseUp(mouseAt(*view,target));
        for(int i=0;i<20;++i)pump();
        require(speed->getValue()==savedSpeed&&sameEdit(edit,p.recording().edits()->front())&&p.recording().snapshot()==audio,"204 resizing overview leaves selected Print parameters and sound intact");
    }
    const auto beforeToggle=p.recording().snapshot();
    auto right=[&]{view->mouseDown(mouseAt(*view,{400,200},true));view->mouseUp(mouseAt(*view,{400,200},true));pump();};
    right();require(box->isVisible()&&box->hasImage(),"204 right shows source");
    right();require(!box->isVisible()&&p.recording().edits()->size()==1&&p.recording().snapshot()==beforeToggle,"204 right hides source without removing or rendering Print");
    view->mouseDown(mouseAt(*view,{400,5}));view->mouseUp(mouseAt(*view,{400,5}));pump();
    right();require(box->isVisible()&&box->hasImage()&&p.recording().edits()->size()==1,"204 source can be shown after deselecting Print");right();
    png(dir.getChildFile("204-standalone.png"),editor->createComponentSnapshot(editor->getLocalBounds()));
    auto* edition=dynamic_cast<juce::Label*>(componentWithID(*editor,"editionLabel"));require(edition&&edition->getText().contains(juce::String::fromUTF8("子青版")),"204 edition label");
    editor.reset(p.createEditor());pump();view=componentWithID(*editor,"spectrumView");box=imageBox(*editor);
    require(!box->isVisible()&&box->hasImage(),"204 hidden source survives editor reopen");
    // Restore selection after deliberately deselecting above.
    if(auto* next=dynamic_cast<juce::Button*>(componentWithID(*editor,"nextPrint")))next->onClick();
    else{auto state=p.getEditorState();state.selectedPrint=id;p.setEditorState(state);editor.reset(p.createEditor());pump();view=componentWithID(*editor,"spectrumView");box=imageBox(*editor);}
    editor->keyPressed(juce::KeyPress(juce::KeyPress::deleteKey));
    wait([&]{return p.recording().edits()->empty()&&!p.recording().isBusy();},"204 Delete removes edit");right();
    require(box->hasImage()&&p.recording().edits()->empty(),"204 source remains reusable after Delete without resurrecting the Print");
    {
        // A real right-click, original-image drag, and explicit Print sequence.
        // Reopening above reconstructed every control, so look them up again.
        approve=dynamic_cast<juce::Button*>(componentWithID(*editor,"imagePrint"));
        cancel=dynamic_cast<juce::Button*>(componentWithID(*editor,"cancelPrint"));
        fill=slider("noiseFill");
        fitSpectrum(*view);pump();
        slider("imageAmount")->setValue(.5,juce::sendNotificationSync);
        wait([&]{return box->conversionReady();},"206 prepare balanced Logo for move and restore");
        box->setNormalisedBounds({.12f,.2f,.16f,.19f});
        auto settled=[&]{return !p.recording().isBusy()&&!cancel->isVisible()&&!(bool)view->getProperties()["previewActive"];};
        auto rightAt=[&](juce::Point<float> at){view->mouseDown(mouseAt(*view,at,true));view->mouseUp(mouseAt(*view,at,true));pump();};
        auto centre=[&]{shot();auto& v=view->getProperties();return juce::Point<float>((float)v["selectedX"]+.5f*(float)v["selectedW"],(float)v["selectedY"]+.5f*(float)v["selectedH"]);};
        auto moveSource=[&](int dx){
            auto at=box->getLocalBounds().getCentre().toFloat();box->mouseDown(mouseAt(*box,at));
            box->mouseDrag(mouseAt(*box,at.translated((float)dx,0)));box->mouseUp(mouseAt(*box,at));
        };
        approve->onClick();
        require(cancel->isVisible(),"205 new Print is cancellable immediately");cancel->onClick();
        wait([&]{return p.recording().edits()->empty()&&!p.recording().isBusy()&&box->isVisible();},"205 cancel first Print returns editable source");
        approve->onClick();wait(settled,"205 first real stamp");
        const auto firstAudio=p.recording().snapshot();
        const auto first=firstAudio->edits.front();
        const auto firstPixels=shot();
        const auto firstArea=area().reduced(3).withTrimmedBottom(10); // Exclude the selected corner resize handle.
        png(dir.getChildFile("205-first.png"),firstPixels);
        require(!firstArea.isEmpty(),"205 first stamp has measurable interior");
        auto click=[&](juce::Point<float> at){view->mouseDown(mouseAt(*view,at));view->mouseUp(mouseAt(*view,at));for(int i=0;i<4;++i)pump();};
        auto overview=[&]{fitSpectrum(*view);for(int i=0;i<5;++i)pump();};
        auto drop=[&](const juce::File& f){dynamic_cast<juce::FileDragAndDropTarget*>(editor.get())->filesDropped({f.getFullPathName()},0,0);};
        rightAt(centre());
        require(box->isVisible()&&!box->spectrumPreview.isValid()&&!approve->isVisible(),"206 source recall is clean and has no Print button");
        moveSource(1);
        require(!approve->isVisible()&&p.getEditorState().selectedPrint==first.id,"207 a click or jitter keeps the recalled source clean");
        moveSource(180);for(int i=0;i<6;++i)pump();
        auto copyState=p.getEditorState();
        require(approve->isVisible()&&copyState.selectedPrint.isEmpty()&&copyState.standaloneCopyOrigin==first.id,"207 source drag places an independent copy");
        require(diff(firstPixels,shot(),firstArea)==0&&p.recording().snapshot()==firstAudio,"207 old stamp stays visible and audible throughout a copy preview");
        click({85,3});overview();
        require(!approve->isVisible()&&p.getEditorState().standaloneCopyOrigin.isEmpty()&&!box->isVisible(),"207 blank click discards pending copy");
        require(diff(firstPixels,shot(),firstArea)==0&&p.recording().snapshot()==firstAudio,"207 discard preserves original pixels and audio");
        for(int count:{2,3}){
            const auto previous=p.recording().snapshot();
            rightAt(centre());overview();
            if(count==2){
                fill->onDragStart();fill->setValue(.75,juce::sendNotificationSync);fill->onDragEnd();
            }
            moveSource(180);for(int i=0;i<5;++i)pump();
            require(p.getEditorState().standaloneCopyOrigin==previous->edits.back().id,"207 repeated copies detach from the last stamp");
            if(count==2){
                approve->onClick();require(cancel->isVisible(),"207 a new copy is cancellable");cancel->onClick();
                wait([&]{return !cancel->isVisible()&&!p.recording().isBusy();},"207 copy cancellation finishes");
                require(p.recording().edits()->size()==1&&sameEdit(first,p.recording().edits()->front())
                    &&p.recording().snapshot()->rendered==previous->rendered&&p.getEditorState().standaloneCopyOrigin==first.id,"207 cancelling a copy leaves original audio and restores the draft");
            }
            require(p.recording().snapshot()->rendered==previous->rendered&&p.recording().edits()->size()==count-1,"205 dragging source leaves original Prints intact");
            approve->onClick();wait(settled,"205 stamp at next position");
            const auto next=p.recording().snapshot();require(next->edits.size()==count&&sameEdit(first,next->edits.front()),"205 source drag adds an independent Print");
            require(next->edits.back().id!=previous->edits.back().id&&next->edits.back().imagePath==first.imagePath&&std::abs(next->edits.back().compensation-.75)<.001,"207 copy carries artwork and edited settings, with a separate identity");
            overview(); // Compare the same timeline pixels after selection recentres the view.
            const auto afterPixels=shot();png(dir.getChildFile("205-after-"+juce::String(count)+".png"),afterPixels);
            require(diff(firstPixels,afterPixels,firstArea)==0,"205 first stamp pixels survive subsequent Prints");
            const auto firstSample=llround(first.start*next->sampleRate),lastSample=llround((first.start+first.duration)*next->sampleRate);
            double signal=0;
            for(auto i=firstSample;i<lastSample;++i){
                const float was=firstAudio->sample(0,i);
                signal+=std::abs(was);
                require(next->sample(0,i)==was,"205 earlier stamp keeps exact rendered samples");
            }
            require(signal>0,"205 preserved stamp has audible content");
            require(std::abs(next->edits.back().start-previous->edits.back().start)>.01,"205 duplicate has a different timeline position");
        }
        fitSpectrum(*view);pump();shot();
        const auto before=p.recording().snapshot();
        const double start=(double)view->getProperties()["displayStart"],end=(double)view->getProperties()["displayEnd"];
        const float hz=(first.low+first.high)*.5f*(float)before->sampleRate*.5f;
        const auto pt=juce::Point<float>(70+(float)((first.start+.5*first.duration-start)/(end-start))*(view->getWidth()-70),
            frequency::toY(hz,(float)view->getProperties()["displayTop"],first.scaleMode)*(view->getHeight()-102));
        rightAt(pt);
        require(box->isVisible()&&p.getEditorState().selectedPrint==first.id,"205 right-click hit selects first Print even while third is selected");
        const auto passes=p.recording().printPasses();
        fill->onDragStart();fill->setValue(.93,juce::sendNotificationSync);fill->onDragEnd();
        for(int i=0;i<26;++i)pump();
        require(box->isVisible()&&p.getEditorState().selectedPrint==first.id&&p.recording().snapshot()==before&&p.recording().printPasses()==passes,"205 visible source keeps focus and never renders on release");
        rightAt(centre());
        require(!box->isVisible()&&approve->isVisible()&&p.getEditorState().selectedPrint==first.id
            &&p.getEditorState().standaloneCopyOrigin.isEmpty()&&std::abs(fill->getValue()-.93)<.001
            &&p.recording().snapshot()==before,"208 right-click from a revealed source with edits keeps the draft in reference mode");
        click({85,3});
        require(p.getEditorState().selectedPrint==first.id&&p.getEditorState().standaloneDraft
            &&std::abs(fill->getValue()-.93)<.001&&p.recording().snapshot()==before
            &&(bool)view->getProperties()["previewActive"],"209 blank click after hiding keeps the current settings until a different Print is chosen");
        fitSpectrum(*view);pump();rightAt(pt);
        fill->onDragStart();fill->setValue(.93,juce::sendNotificationSync);fill->onDragEnd();
        fitSpectrum(*view);for(int i=0;i<4;++i)pump();shot();
        const auto second=before->edits[1];
        const float secondHz=(second.low+second.high)*.25f*(float)before->sampleRate;
        auto& vp=view->getProperties();
        const float x2=70+(float)((second.start+second.duration*.5-(double)vp["displayStart"])/((double)vp["displayEnd"]-(double)vp["displayStart"]))*(view->getWidth()-70);
        click({x2,frequency::toY(secondHz,(float)vp["displayTop"],second.scaleMode)*(view->getHeight()-102)});
        require(p.getEditorState().selectedPrint==second.id&&!p.getEditorState().standaloneDraft
            &&std::abs(fill->getValue()-second.compensation)<.001&&p.recording().snapshot()==before
            &&p.recording().printPasses()==passes,"205 clicking a different stamp shows its saved parameters without rendering");
        fitSpectrum(*view);pump();rightAt(pt);
        fill->onDragStart();fill->setValue(.93,juce::sendNotificationSync);fill->onDragEnd();
        approve->onClick();
        require(cancel->isVisible(),"205 replacement Print is cancellable");cancel->onClick();
        wait([&]{return !cancel->isVisible()&&!p.recording().isBusy();},"205 replacement cancellation finishes");
        const auto rolledBack=p.recording().snapshot();
        require(rolledBack->edits.size()==3&&sameEdit(first,rolledBack->edits.front())&&rolledBack->rendered==before->rendered&&rolledBack->spectrum==before->spectrum,"205 cancel restores all original PCM and FFT pages");
        approve->onClick();wait(settled,"205 retry after cancellation");
        require(p.recording().edits()->size()==3&&p.recording().edits()->front().compensation>.9f,"205 retry replaces correct Print");
        const auto original=p.recording().snapshot();
        rightAt(centre());require(!approve->isVisible()&&!box->spectrumPreview.isValid(),"206 right click restores Logo B/W");
        auto* amount=slider("imageAmount");amount->onDragStart();amount->setValue(.23,juce::sendNotificationSync);
        wait([&]{return box->spectrumPreview.isValid()&&approve->isVisible();},"206 Logo becomes simulated during adjustment");
        require(p.recording().snapshot()==original,"206 Logo parameter drag is visual only");amount->onDragEnd();
        click({85,3});overview();rightAt(pt);
        const auto oldArt=box->getImagePath();
        auto replacementDir=dir.getChildFile("alternate-pictures");replacementDir.createDirectory();
        auto replacement=replacementDir.getChildFile("logo.png");
        juce::Image stripe(juce::Image::RGB,90,45,true);
        {juce::Graphics g(stripe);g.fillAll(juce::Colours::white);g.setColour(juce::Colours::black);g.fillRect(10,0,28,45);}
        png(replacement,stripe);drop(replacement);
        wait([&]{return box->conversionReady()&&box->isVisible()&&!box->spectrumPreview.isValid()&&approve->isVisible();},"209 replacement offers Print with the converted source at rest");
        require(Preferences::imageChooserDirectory()==replacementDir&&box->getImagePath()!=oldArt,"206 chooser remembers actual source folder");
        require(p.getEditorState().standaloneDraft->id==first.id&&p.recording().snapshot()==original,"206 replacing source is a draft on the selected print");
        click({85,3});
        require(box->getImagePath()==oldArt&&p.recording().snapshot()==original&&!p.getEditorState().standaloneDraft,"206 cancel replacement recalls original artwork");
        overview();rightAt(pt);drop(replacement);
        approve->onClick();wait(settled,"207 explicit in-place replacement commits");
        const auto result=p.recording().snapshot();
        require(result->edits.size()==3&&result->edits.front().id==first.id&&result->edits.front().imagePath!=oldArt
            &&std::abs(result->edits.front().imageAspect-2.f)<.001f
            &&sameEdit(result->edits[1],original->edits[1])&&sameEdit(result->edits[2],original->edits[2]),"207 replacing the source without dragging still edits only the chosen Print");
        drop(replacement);for(int i=0;i<5;++i)pump();
        require(approve->isVisible()&&!box->spectrumPreview.isValid(),"206 fresh Logo shows B/W instead of simulation");
        auto bad=dir.getChildFile("invalid.png");bad.replaceWithText("not an image");drop(bad);
        require(Preferences::imageChooserDirectory()==replacementDir&&box->hasImage(),"206 invalid image retains chooser folder and source");
        // Capture the finished underlying stamp, add an overlapping Print, then
        // lift it in a move preview. Its old footprint must match that capture.
        rightAt({85,3});overview();
        const auto underlay=shot();const auto under=result->edits.front();
        auto& props=view->getProperties();
        const double viewStart=(double)props["displayStart"],span=(double)props["displayEnd"]-viewStart;
        const float nyquist=(float)result->sampleRate*.5f,top=(float)props["displayTop"];
        auto y=[&](float hzNorm){return frequency::toY(hzNorm*nyquist,top,under.scaleMode);};
        drop(replacement);
        box->setPrintBounds({(float)((under.start-viewStart)/span)+.015f,y(under.high),(float)(under.duration/span),y(under.low)-y(under.high)});
        if(box->onChanged)box->onChanged();
        fill->setValue(0,juce::sendNotificationSync);
        approve->onClick();wait(settled,"206 overlapping Print finished");
        auto overlapAudio=p.recording().snapshot();require(overlapAudio->edits.size()==4,"206 new overlay is independent");
        const auto overlapArea=area().reduced(3).withTrimmedBottom(10);
        require(diff(underlay,shot(),overlapArea)>15,"206 overlapping stamp visibly changes the reference region");
        const auto printedCentre=centre(),destination=printedCentre.translated(230.f,0.f);
        view->mouseDown(mouseAt(*view,printedCentre));view->mouseDrag(mouseAt(*view,destination));view->mouseUp(mouseAt(*view,destination));
        require(p.getEditorState().standaloneDraft&&p.getEditorState().standaloneDraft->id==overlapAudio->edits.back().id,"207 direct pattern drag keeps the original identity");
        overview();
        require(diff(underlay,shot(),overlapArea)==0&&p.recording().snapshot()==overlapAudio,"206 moving top stamp reveals exact pre-Print pixels with the underlying stamp intact");
        rightAt({85,3});
        require(box->isVisible()&&!box->spectrumPreview.isValid()&&approve->isVisible()&&p.getEditorState().selectedPrint.isEmpty()
            &&box->getNormalisedBounds().getCentre().getDistanceFrom({.5f,.5f})<.003f,"209 blank right-click detaches a centred, printable source even with a selected draft");
        moveSource(35);
        approve->onClick();wait(settled,"209 printing a centred source completes");
        const auto centred=p.recording().edits();
        require(!box->isVisible()&&centred->size()==5&&std::equal(overlapAudio->edits.begin(),overlapAudio->edits.end(),centred->begin(),sameEdit),
            "209 centred copy adds one Print and preserves every existing stamp");
        std::cout<<"PASS 209 centre placement, mode-specific release, and hidden parameter retention\n";
        std::cout<<"PASS 206 overlapping Print background matches the actual pre-Print spectrum\n";
        std::cout<<"PASS 206 source recall, Logo simulation, folder recall, move and replace drafts, explicit commit and discard\n";
    }
    editor.reset();
    // Compare the actual Logo component against the original 1.4.28 drawing
    // operations at a non-native size, including antialiased edge pixels.
    ImageBoxComponent logo;logo.setLivePreviewMode(true);
    juce::Image logoArt(juce::Image::ARGB,74,43,true);
    {juce::Graphics g(logoArt);g.setColour(juce::Colours::black);g.fillEllipse(5.f,4.f,33.f,31.f);g.setColour(juce::Colours::grey);g.fillRect(43,1,12,42);}
    const auto logoFile=dir.getChildFile("204-logo.png");png(logoFile,logoArt);logo.loadImageFromFile(logoFile);
    logo.setBounds(0,0,155,95);
    for(bool invert:{false,true})for(float ratio:{0.f,.17f,.33f}){
        logo.setConversion(2,.55f,invert,ratio,false,.5f,.5f);
        wait([&]{logo.refreshConversion();return logo.conversionReady();},"204 Logo conversion");
        const auto mask=image::binarize(logoArt,{2,.55f});
        juce::Image expected(juce::Image::RGB,155,95,true);
        {juce::Graphics g(expected);const auto shade=(juce::uint8)juce::roundToInt(255*std::sqrt(ratio));
         const juce::Colour dim(shade,shade,shade);
         g.fillAll(invert?dim:juce::Colours::white);g.setColour(invert?juce::Colours::white:dim);
         g.drawImage(mask,expected.getBounds().toFloat(),juce::RectanglePlacement::centred,true);}
        const auto actual=logo.createComponentSnapshot(logo.getLocalBounds());
        for(int y=4;y<77;++y)for(int x=4;x<151;++x)
            require(actual.getPixelAt(x,y)==expected.getPixelAt(x,y),"204 Logo interior matches 1.4.28 pixel for pixel");
    }
    std::cout<<"PASS 204 Logo source preview matches 1.4.28 pixels in six settings\n";
    // Compare complex-image prediction with the actual STFT of rendered audio,
    // including non-empty material (the upstream black-overlay mismatch).
    std::mt19937 rng(204);std::normal_distribution<float> white(0,.04f);
    for(int n=0;n<source.getNumSamples();++n)source.setSample(0,n,white(rng)+.08f*std::sin(n*.093f));
    p.recording().clear();require(p.recording().flush(),"204 reset accuracy source");
    p.recording().setArmed(true);p.recording().capture(source,0,48000,true);p.recording().setArmed(false);require(p.recording().flush(),"204 accuracy source");
    PrintEdit e;e.start=.2;e.duration=1.5;e.low=.08f;e.high=.8f;e.fftSize=4096;e.rows=2049;e.cols=160;e.scaleMode=4;e.imageStyle=1;
    auto mask=std::make_shared<std::vector<float>>(e.rows*e.cols);
    for(int y=0;y<e.rows;++y)for(int x=0;x<e.cols;++x){
        // Smooth tone plus narrow ink lines and a solid silhouette against texture.
        const float u=(x+.5f)/e.cols,v=(y+.5f)/e.rows;
        const float ellipse=std::pow((u-.48f)/.24f,2.f)+std::pow((v-.48f)/.32f,2.f);
        const float ink=(ellipse>.93f&&ellipse<1.06f)||(ellipse<1.f&&v>.39f&&v<.43f)?1.f:.1f+.85f*u;
        (*mask)[(size_t)y*e.cols+x]=ink;
    }
    e.mask=mask;
    {
        // Deterministic music-like material with two overlapping opaque Prints.
        // Inspect the very first stopped paint after a real Delete keystroke.
        auto first=e;first.id="under";first.start=.20;first.duration=.80;first.low=.10f;first.high=.23f;
        first.mask=std::make_shared<const std::vector<float>>(first.rows*first.cols,1.f);
        auto top=first;top.id="top";top.start=.45;top.duration=.65;top.low=.17f;top.high=.44f;
        first.imagePath=top.imagePath=file.getFullPathName();
        p.recording().add(first);p.recording().add(top);require(p.recording().flush(),"204 overlapping prints ready");
        auto state=p.getEditorState();state.imageVisible=false;state.selectedPrint=top.id;state.placementId=top.id;
        state.viewFollow=false;state.viewFit=true;state.viewValid=true;p.setEditorState(state);
        std::unique_ptr<juce::AudioProcessorEditor> deletionEditor(p.createEditor());auto* canvas=componentWithID(*deletionEditor,"spectrumView");
        wait([&]{return (bool)canvas->getProperties()["recordMode"];},"204 overlap canvas");
        fitSpectrum(*canvas);pump();
        wait([&]{return (int)canvas->getProperties()["rasterRevision"]==p.recording().snapshot()->revision;},"204 before deletion raster");
        auto compareDeletion=[&](const PrintEdit& removed){
            const auto& props=canvas->getProperties();const double start=(double)props["displayStart"],end=(double)props["displayEnd"];
            const int width=canvas->getWidth()-70,height=canvas->getHeight()-102;
            const auto xAt=[&](double t){return 70+juce::roundToInt((t-start)/(end-start)*width);};
            const auto yAt=[&](float f){return juce::roundToInt(frequency::toY(f*24000,(float)props["displayTop"],(int)props["scaleMode"])*(height-1));};
            const int x=xAt(.72),outside=yAt(.55f),inside=yAt(.205f),restored=yAt(.34f);
            auto mean=[&](const juce::Image& im,int y){double sum=0;for(int dy=-2;dy<=2;++dy)for(int dx=-2;dx<=2;++dx)sum+=im.getPixelAt(x+dx,y+dy).getBrightness();return sum/25.;};
            deletionEditor->keyPressed(juce::KeyPress(juce::KeyPress::deleteKey));
            auto immediate=canvas->createComponentSnapshot(canvas->getLocalBounds());
            require(mean(immediate,outside)>.15,"204 Delete preserves frequency content above the Print on its first paint");
            require(mean(immediate,restored)>.15,"204 Delete restores audio at the removed Print immediately");
            if(removed.id==top.id)require(mean(immediate,inside)<.10,"204 underlying overlapping Print remains attenuated");
            wait([&]{auto snap=p.recording().snapshot();return !p.recording().isBusy()&&(int)props["rasterRevision"]==snap->revision&&snap->edits.size()==p.recording().edits()->size();},"204 exact deletion raster");
            auto exact=canvas->createComponentSnapshot(canvas->getLocalBounds());
            require(std::abs(mean(immediate,outside)-mean(exact,outside))<.025&&std::abs(mean(immediate,inside)-mean(exact,inside))<.08,"204 immediate patch matches settled spectrum");
        };
        compareDeletion(top);
        auto* next=dynamic_cast<juce::Button*>(componentWithID(*deletionEditor,"nextPrint"));next->onClick();
        fitSpectrum(*canvas);pump();
        compareDeletion(first);
        std::cout<<"PASS 204 immediate Delete restores recorded spectrum and preserves overlapping Prints\n";
    }
    for(const float compensation:{0.f,.7f}){
        if(compensation>0){
            p.recording().clear();require(p.recording().flush(),"204 quiet source reset");
            source.applyGain(.0001f);p.recording().setArmed(true);p.recording().capture(source,0,48000,true);p.recording().setArmed(false);require(p.recording().flush(),"204 quiet source for fill prediction");
        }
        e.compensation=compensation;p.recording().add(e,e.id);require(p.recording().flush(),"204 accuracy render");
        const auto audio=p.recording().snapshot();auto predicted=printPreview(*audio,e,3,true);
        double error=0,oldError=0;int count=0;juce::Image actual(juce::Image::RGB,predicted.getWidth(),predicted.getHeight(),true);
        for(int y=0;y<actual.getHeight();++y)for(int x=0;x<actual.getWidth();++x){
            const double t=e.start+(x+.5)*e.duration/actual.getWidth();const auto* wet=audio->frameAt(t);const auto* dry=audio->frameAt(t,false);
            if(!wet||!dry)continue;
            const float frequency=spectrumtag::frequency::interpolate(1.f-y/(float)(actual.getHeight()-1),e.low*24000,e.high*24000,e.scaleMode);
            const float bin=frequency/24000*(wet->bins-1);const int lo=(int)bin,hi=juce::jmin(wet->bins-1,lo+1);
            const auto level=[&](const SpectrumFrame* f){return juce::jlimit(0,255,juce::roundToInt(f->level[lo]+(bin-lo)*(f->level[hi]-f->level[lo])));};
            const auto real=spectrumColour(3,level(wet)/255.f);actual.setPixelAt(x,y,real);
            if(x>8&&x<actual.getWidth()-9&&y>8&&y<actual.getHeight()-9){
                error+=std::abs(real.getBrightness()-predicted.getPixelAt(x,y).getBrightness());
                const int maskY=juce::jlimit(0,e.rows-1,juce::roundToInt((frequency/24000-e.low)/(e.high-e.low)*(e.rows-1)));
                const float affected=(*e.mask)[(size_t)maskY*e.cols+x];
                oldError+=std::abs(real.getBrightness()-spectrumColour(3,level(dry)/255.f).getBrightness()*(1-affected));++count;
            }
        }
        const auto suffix=compensation>0?"fill":"no-fill";
        png(dir.getChildFile(juce::String("204-accuracy-preview-")+suffix+".png"),predicted);
        png(dir.getChildFile(juce::String("204-accuracy-render-")+suffix+".png"),actual);
        std::cout<<"204_ACCURACY "<<suffix<<" mean_brightness_error="<<error/count<<" old_overlay_error="<<oldError/count<<"\n"<<std::flush;
        require(error/count<.08&&error<oldError*.6,"204 complex prediction is closer to audio than black overlay");
    }
    std::cout<<"2.0.10 PRINT AND PREVIEW CHECKS PASSED\n"<<std::flush;
}
