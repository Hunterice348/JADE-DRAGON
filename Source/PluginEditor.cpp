#include "PluginEditor.h"
#include <cmath>
#include <vector>
#include <algorithm>

namespace
{
    const juce::Colour black     { 0xff061411 };
    const juce::Colour deepJade  { 0xff092a23 };
    const juce::Colour jade      { 0xff147d65 };
    const juce::Colour lightJade { 0xff65e8bd };
    const juce::Colour gold      { 0xffd9b66e };
    const juce::Colour paleGold  { 0xffffe6a8 };
    const juce::Colour ivory     { 0xffeee9d6 };
}

// ===================================================================
// LOOK & FEEL
// ===================================================================

JadeLookAndFeel::JadeLookAndFeel()
{
    setColour(juce::Slider::textBoxTextColourId, ivory);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff102a23));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff102e27));
    setColour(juce::ComboBox::textColourId, ivory);
    setColour(juce::ComboBox::outlineColourId, gold);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff102a23));
    setColour(juce::PopupMenu::textColourId, ivory);
}

void JadeLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                       float position, float startAngle, float endAngle, juce::Slider&)
{
    const float size = static_cast<float>(juce::jmin(width, height) - 18);
    const float cx = x + width * 0.5f;
    const float cy = y + height * 0.5f - 7.0f;
    const float radius = size * 0.5f;
    const float angle = startAngle + position * (endAngle - startAngle);
    const juce::Rectangle<float> knob(cx - radius, cy - radius, size, size);

    g.setColour(juce::Colours::black.withAlpha(0.60f));
    g.fillEllipse(knob.translated(0.0f, 5.0f));

    g.setGradientFill(juce::ColourGradient(paleGold, cx - radius, cy - radius,
        juce::Colour(0xff70502a), cx + radius, cy + radius, false));
    g.fillEllipse(knob);

    const auto inner = knob.reduced(4.0f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff39b894),
        cx - radius * 0.55f, cy - radius * 0.65f, juce::Colour(0xff061d18),
        cx + radius * 0.55f, cy + radius * 0.65f, false));
    g.fillEllipse(inner);

    juce::Path highlightArc;
    highlightArc.addArc(inner.getX() + 5.0f, inner.getY() + 5.0f,
        inner.getWidth() - 10.0f, inner.getHeight() - 10.0f,
        juce::MathConstants<float>::pi * 1.15f,
        juce::MathConstants<float>::pi * 1.85f, true);
    g.strokePath(highlightArc, juce::PathStrokeType(2.0f));

    for (int i = 0; i <= 20; ++i)
    {
        const float a = startAngle + (endAngle - startAngle) * static_cast<float>(i) / 20.0f;
        const float outer = radius + 5.0f;
        const float innerTick = radius + 1.0f;
        const float ha = a - juce::MathConstants<float>::halfPi;
        const juce::Point<float> p1(cx + std::cos(ha) * outer, cy + std::sin(ha) * outer);
        const juce::Point<float> p2(cx + std::cos(ha) * innerTick, cy + std::sin(ha) * innerTick);
        g.setColour(i <= static_cast<int>(position * 20.0f) ? paleGold : gold.withAlpha(0.35f));
        g.drawLine(p1.x, p1.y, p2.x, p2.y, i % 5 == 0 ? 2.0f : 1.0f);
    }

    const float indicatorLength = radius * 0.66f;
    const float ia = angle - juce::MathConstants<float>::halfPi;
    const float ix = cx + std::cos(ia) * indicatorLength;
    const float iy = cy + std::sin(ia) * indicatorLength;
    g.setColour(paleGold);
    g.drawLine(cx, cy, ix, iy, 3.2f);
    g.setColour(black);
    g.fillEllipse(cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
    g.setColour(paleGold);
    g.fillEllipse(cx - 2.0f, cy - 2.0f, 4.0f, 4.0f);
}

// ===================================================================
// EDITOR
// ===================================================================

JADEDRAGONAudioProcessorEditor::JADEDRAGONAudioProcessorEditor(JADEDRAGONAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&jadeLook);
    setSize(1120, 760);

    titleLabel.setText("JADE DRAGON", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(juce::FontOptions(36.0f, juce::Font::bold)));
    titleLabel.setColour(juce::Label::textColourId, paleGold);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("PREMIUM CHINESE INSTRUMENT ENGINE", juce::dontSendNotification);
    subtitleLabel.setJustificationType(juce::Justification::centred);
    subtitleLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
    subtitleLabel.setColour(juce::Label::textColourId, lightJade);
    addAndMakeVisible(subtitleLabel);

    instrumentSelector.addItem("GUZHENG", 1);
    instrumentSelector.addItem("ERHU", 2);
    instrumentSelector.addItem("DIZI", 3);
    instrumentSelector.addItem("PIPA", 4);
    instrumentSelector.addItem("GONG", 5);
    instrumentSelector.addItem("JADE BELLS", 6);
    addAndMakeVisible(instrumentSelector);

    setupSlider(attack, "ATTACK");
    setupSlider(decay, "DECAY");
    setupSlider(sustain, "SUSTAIN");
    setupSlider(release, "RELEASE");
    setupSlider(brightness, "BRIGHTNESS");
    setupSlider(drive, "DRIVE");
    setupSlider(chorus, "CHORUS");
    setupSlider(reverb, "REVERB");
    setupSlider(delay, "DELAY");
    setupSlider(master, "MASTER");

    auto& state = processor.parameters;
    instrumentAttachment = std::make_unique<ComboAttachment>(state, "instrument", instrumentSelector);
    attackAttachment = std::make_unique<SliderAttachment>(state, "attack", attack);
    decayAttachment = std::make_unique<SliderAttachment>(state, "decay", decay);
    sustainAttachment = std::make_unique<SliderAttachment>(state, "sustain", sustain);
    releaseAttachment = std::make_unique<SliderAttachment>(state, "release", release);
    brightnessAttachment = std::make_unique<SliderAttachment>(state, "brightness", brightness);
    driveAttachment = std::make_unique<SliderAttachment>(state, "drive", drive);
    chorusAttachment = std::make_unique<SliderAttachment>(state, "chorus", chorus);
    reverbAttachment = std::make_unique<SliderAttachment>(state, "reverb", reverb);
    delayAttachment = std::make_unique<SliderAttachment>(state, "delay", delay);
    masterAttachment = std::make_unique<SliderAttachment>(state, "master", master);

    startTimerHz(60);
}

JADEDRAGONAudioProcessorEditor::~JADEDRAGONAudioProcessorEditor() { setLookAndFeel(nullptr); }

void JADEDRAGONAudioProcessorEditor::setupSlider(juce::Slider& slider, const juce::String& name)
{
    slider.setName(name);
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 20);
    slider.setNumDecimalPlacesToDisplay(2);
    addAndMakeVisible(slider);
}

// ===================================================================
// 3D JADE CORE VISUALIZER
// ===================================================================

void JADEDRAGONAudioProcessorEditor::draw3DJadeCore(juce::Graphics& g,
                                                     juce::Rectangle<float> area, float audioLevel)
{
    const float cx = area.getCentreX();
    const float cy = area.getCentreY();
    const float baseRadius = area.getWidth() * 0.35f;
    const float fov = 400.0f;

    struct ProjectedPoint { float x, y, z, brightness; };
    std::vector<ProjectedPoint> points;
    points.reserve(40 * 40);

    const int rings = 40, sides = 40;
    const float p = 2.0f, q = 3.0f;
    const float R = baseRadius * 0.6f;
    const float r = baseRadius * 0.25f * (1.0f + audioLevel * 0.5f);

    const float rotY = rotation;
    const float rotX = rotation * 0.5f + audioLevel * 0.2f;
    const float cosY = std::cos(rotY), sinY = std::sin(rotY);
    const float cosX = std::cos(rotX), sinX = std::sin(rotX);

    for (int i = 0; i < rings; ++i)
    {
        const float u = (juce::MathConstants<float>::twoPi * i) / rings;
        for (int j = 0; j < sides; ++j)
        {
            const float v = (juce::MathConstants<float>::twoPi * j) / sides;
            const float x = (R + r * std::cos(q * v)) * std::cos(p * u);
            const float y = (R + r * std::cos(q * v)) * std::sin(p * u);
            const float z = r * std::sin(q * v);

            const float x1 = x * cosY - z * sinY;
            const float z1 = x * sinY + z * cosY;
            const float y1 = y * cosX - z1 * sinX;
            const float z2 = y * sinX + z1 * cosX;

            const float scale = fov / (fov + z2 + baseRadius);
            points.push_back({ cx + x1 * scale, cy + y1 * scale, z2, scale });
        }
    }

    std::sort(points.begin(), points.end(),
        [](const ProjectedPoint& a, const ProjectedPoint& b) { return a.z > b.z; });

    for (const auto& pt : points)
    {
        const float zNorm = (pt.z + baseRadius) / (baseRadius * 2.0f);
        const float alpha = juce::jlimit(0.2f, 1.0f, zNorm);
        const float size = juce::jlimit(1.0f, 3.5f, pt.brightness * 2.5f) * (1.0f + audioLevel);
        juce::Colour c = (pt.z > 0) ? paleGold : lightJade;
        g.setColour(c.withAlpha(alpha * 0.8f));
        g.fillEllipse(pt.x - size * 0.5f, pt.y - size * 0.5f, size, size);
    }

    g.setColour(lightJade.withAlpha(0.15f + audioLevel * 0.2f));
    g.drawEllipse(cx - baseRadius * 0.8f, cy - baseRadius * 0.8f,
                  baseRadius * 1.6f, baseRadius * 1.6f, 1.5f);
}

// ===================================================================
// PAINT
// ===================================================================

void JADEDRAGONAudioProcessorEditor::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff143c30), 0.0f, 0.0f,
        black, 0.0f, static_cast<float>(getHeight()), false));
    g.fillAll();

    g.setColour(lightJade.withAlpha(0.035f));
    for (int x = 0; x < getWidth(); x += 36)
        g.drawVerticalLine(x, 0.0f, static_cast<float>(getHeight()));
    for (int y = 0; y < getHeight(); y += 36)
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(getWidth()));

    g.setColour(gold.withAlpha(0.85f));
    g.drawRoundedRectangle(bounds.reduced(9.0f), 18.0f, 2.0f);
    g.setColour(paleGold.withAlpha(0.18f));
    g.drawRoundedRectangle(bounds.reduced(15.0f), 15.0f, 1.0f);

    const float corner = 38.0f;
    g.setColour(gold);
    for (int sx : { 1, -1 })
    {
        for (int sy : { 1, -1 })
        {
            const float x = sx > 0 ? 20.0f : static_cast<float>(getWidth()) - 20.0f;
            const float y = sy > 0 ? 20.0f : static_cast<float>(getHeight()) - 20.0f;
            g.drawLine(x, y, x + sx * corner, y, 2.0f);
            g.drawLine(x, y, x, y + sy * corner, 2.0f);
        }
    }

    titleLabel.setBounds(0, 20, getWidth(), 48);
    subtitleLabel.setBounds(0, 67, getWidth(), 18);

    g.setColour(deepJade.withAlpha(0.88f));
    g.fillRoundedRectangle(24.0f, 270.0f, static_cast<float>(getWidth()) - 48.0f, 205.0f, 15.0f);
    g.fillRoundedRectangle(24.0f, 500.0f, static_cast<float>(getWidth()) - 48.0f, 210.0f, 15.0f);

    g.setColour(gold.withAlpha(0.4f));
    g.drawRoundedRectangle(24.0f, 270.0f, static_cast<float>(getWidth()) - 48.0f, 205.0f, 15.0f, 1.0f);
    g.drawRoundedRectangle(24.0f, 500.0f, static_cast<float>(getWidth()) - 48.0f, 210.0f, 15.0f, 1.0f);

    smoothLevel += (processor.currentLevel.load() - smoothLevel) * 0.2f;
    draw3DJadeCore(g, juce::Rectangle<float>(
        static_cast<float>(getWidth()) * 0.5f - 120.0f, 90.0f, 240.0f, 170.0f), smoothLevel);

    g.setColour(paleGold);
    g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
    g.drawText("01  |  INSTRUMENT CHARACTER", 45, 280, 400, 20, juce::Justification::left);
    g.drawText("02  |  SOUND DESIGN & EFFECTS", 45, 510, 400, 20, juce::Justification::left);

    g.setColour(lightJade.withAlpha(0.7f));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("HUNTERICE RECORDS     |     JADE DRAGON V2.1", 0,
               getHeight() - 25, getWidth(), 15, juce::Justification::centred);
}

void JADEDRAGONAudioProcessorEditor::resized()
{
    const int w = getWidth();
    instrumentSelector.setBounds(w / 2 - 100, 230, 200, 29);

    const int sliderWidth = 160, sliderHeight = 150, gap = 20;
    const int totalW = 5 * sliderWidth + 4 * gap;
    const int startX = (w - totalW) / 2;

    juce::Slider* top[] = { &attack, &decay, &sustain, &release, &brightness };
    juce::Slider* bottom[] = { &drive, &chorus, &reverb, &delay, &master };

    for (int i = 0; i < 5; ++i)
        top[i]->setBounds(startX + i * (sliderWidth + gap), 310, sliderWidth, sliderHeight);
    for (int i = 0; i < 5; ++i)
        bottom[i]->setBounds(startX + i * (sliderWidth + gap), 535, sliderWidth, sliderHeight);
}

void JADEDRAGONAudioProcessorEditor::timerCallback()
{
    rotation += 0.015f;
    if (rotation >= juce::MathConstants<float>::twoPi)
        rotation -= juce::MathConstants<float>::twoPi;
    repaint();
}
