#pragma once
// 2.0.2：右键是没有菜单的上下文操作。
// 状态1（未打印，有源图）→ 打印；状态3（聚焦）→ 退回图片模式；
// 没有聚焦对象时 → 聚焦离点击位置最近的已打印图形。
static void revision201Checks(const juce::File& dir){
    using namespace spectrumtag;
    SpectrumTagAudioProcessor p;Transport host;p.setPlayHead(&host);p.setRateAndBufferSizeDetails(48000,256);p.prepareToPlay(48000,256);
    std::unique_ptr<juce::AudioProcessorEditor> editor(p.createEditor());tickUI();
    auto* view=componentWithID(*editor,"spectrumView");auto* box=imageBox(*editor);
    require(view&&box,"201 editor parts");
    auto rightClick=[&](juce::Point<float> at){
        view->mouseDown(mouseAt(*view,at,true));view->mouseUp(mouseAt(*view,at,true));tickUI();
    };
    auto drop=[&]{
        dynamic_cast<juce::FileDragAndDropTarget*>(editor.get())->filesDropped({dir.getChildFile("transparent-detail.png").getFullPathName()},0,0);tickUI();
    };
    auto boxCentre=[&]{return view->getLocalPoint(box,box->getLocalBounds().getCentre()).toFloat();};
    // 空工作区：右键什么也不做。
    rightClick({view->getWidth()*.5f,view->getHeight()*.4f});
    require(!box->hasImage()&&(!p.recording().edits()||p.recording().edits()->empty()),"201 right-click on an empty workspace does nothing");
    p.recording().setArmed(true);
    juce::AudioBuffer<float> source(2,48000*6);
    for(int c=0;c<2;++c)for(int i=0;i<source.getNumSamples();++i)source.setSample(c,i,.1f*std::sin(i*.017f));
    p.recording().capture(source,0,48000,true);p.recording().setArmed(false);
    require(p.recording().flush(),"201 source capture");tickUI();fitSpectrum(*view);tickUI();
    drop();require(box->hasImage()&&box->isVisible(),"drop shows artwork in the plugin");
    box->setNormalisedBounds({.05f,.1f,.2f,.4f});tickUI();
    rightClick(boxCentre());
    require(!box->isVisible()&&p.recording().edits()->empty(),"right-click hides artwork without printing");
    rightClick({view->getWidth()*.5f,view->getHeight()*.4f});
    require(box->isVisible()&&p.recording().edits()->empty(),"right-click on empty space centres the reusable image");
    auto* apply=dynamic_cast<juce::Button*>(componentWithID(*editor,"imagePrint"));
    require(apply&&apply->isVisible(),"explicit Print control available");
    apply->onClick();require(p.recording().flush(),"recorded render finishes");tickUI();
    auto edits=p.recording().edits();require(edits->size()==1&&!box->isVisible(),"explicit Print hides the source and commits once");
    const auto original=edits->front();
    rightClick(boxCentre());
    require(box->isVisible()&&box->hasImage()&&sameEdit(p.recording().edits()->front(),original),"revealing a printed source keeps its audio");
    rightClick(boxCentre());
    require(!box->isVisible()&&sameEdit(p.recording().edits()->front(),original),"hiding a printed source preserves its identity");
    std::cout<<"PASS shared right-click and explicit Print in recorded plugin mode\n";
}
