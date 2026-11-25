#include "PluginEditor.h"

namespace
{
    const juce::Colour black       { 0xff061411 };
    const juce::Colour deepJade    { 0xff092a23 };
    const juce::Colour jade        { 0xff147d65 };
    const juce::Colour lightJade   { 0xff65e8bd };
    const juce::Colour gold        { 0xffd9b66e };
    const juce::Colour paleGold    { 0xffffe6a8 };
    const juce::Colour ivory       { 0xffeee9d6 };
}

JadeLookAndFeel::JadeLookAndFeel()
{
    setColour(
        juce::Slider::textBoxTextColourId,
        ivory
    );

    setColour(
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour(0xff102a23)
    );

    setColour(
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::transparentBlack
    );

    setColour(
        juce::ComboBox::backgroundColourId,
        juce::Colour(0xff102e27)
    );

    setColour(
        juce::ComboBox::textColourId,
        ivory
    );

    setColour(
        juce::ComboBox::outlineColourId,
        gold
    );

    setColour(
        juce::PopupMenu::backgroundColourId,
        juce::Colour(0xff102a23)
    );

    setColour(
        juce::PopupMenu::textColourId,
        ivory
    );
}

void JadeLookAndFeel::drawRotarySlider(
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float position,
    float startAngle,
    float endAngle,
    juce::Slider&
)
{
    const float size = static_cast<float>(
        juce::jmin(width, height) - 18
    );

    const float cx = x + width * 0.5f;
    const float cy = y + height * 0.5f - 7.0f;

    const float radius = size * 0.5f;

    const float angle =
        startAngle + position * (endAngle - startAngle);

    const juce::Rectangle<float> knob(
        cx - radius,
        cy - radius,
        size,
        size
    );

    // Deep ambient shadow.
    g.setColour(juce::Colours::black.withAlpha(0.60f));
    g.fillEllipse(knob.translated(0.0f, 5.0f));

    // Outer metal ring.
    g.setGradientFill(
        juce::ColourGradient(
            paleGold,
            cx - radius,
            cy - radius,
            juce::Colour(0xff70502a),
            cx + radius,
            cy + radius,
            false
        )
    );

    g.fillEllipse(knob);

    // Inner jade body.
    const auto inner = knob.reduced(4.0f);

    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xff39b894),
            cx - radius * 0.55f,
            cy - radius * 0.65f,
            juce::Colour(0xff061d18),
            cx + radius * 0.55f,
            cy + radius * 0.65f,
            false
        )
    );

    g.fillEllipse(inner);

    // 3D inner highlight.
    g.setColour(lightJade.withAlpha(0.22f));

    g.drawArc(
        inner.getX() + 5.0f,
        inner.getY() + 5.0f,
        inner.getWidth() - 10.0f,
        inner.getHeight() - 10.0f,
        juce::MathConstants<float>::pi * 1.15f,
        juce::MathConstants<float>::pi * 1.85f,
        true,
        juce::PathStrokeType(2.0f)
    );

    // Position ticks.
    for (int i = 0; i <= 20; ++i)
    {
        const float a =
            startAngle +
            (endAngle - startAngle) *
            static_cast<float>(i) / 20.0f;

        const float outer = radius + 5.0f;
        const float innerTick = radius + 1.0f;

        const juce::Point<float> p1(
            cx + std::cos(a - juce::MathConstants<float>::halfPi)
                * outer,
            cy + std::sin(a - juce::MathConstants<float>::halfPi)
                * outer
        );

        const juce::Point<float> p2(
            cx + std::cos(a - juce::MathConstants<float>::halfPi)
                * innerTick,
            cy + std::sin(a - juce::MathConstants<float>::halfPi)
                * innerTick
        );

        g.setColour(
            i <= static_cast<int>(position * 20.0f)
                ? paleGold
                : gold.withAlpha(0.35f)
        );

        g.drawLine(
            p1.x, p1.y, p2.x, p2.y,
            i % 5 == 0 ? 2.0f : 1.0f
        );
    }

    // Gold indicator.
    const float indicatorLength = radius * 0.66f;

    const float ix =
        cx + std::cos(angle - juce::MathConstants<float>::halfPi)
        * indicatorLength;

    const float iy =
        cy + std::sin(angle - juce::MathConstants<float>::halfPi)
        * indicatorLength;

    g.setColour(paleGold);

    g.drawLine(
        cx,
        cy,
        ix,
        iy,
        3.2f
    );

    g.setColour(black);
    g.fillEllipse(cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);

    g.setColour(paleGold);
    g.fillEllipse(cx - 2.0f, cy - 2.0f, 4.0f, 4.0f);
}

// ============================================================
// EDITOR
// ============================================================

JADEDRAGONAudioProcessorEditor::JADEDRAGONAudioProcessorEditor(
    JADEDRAGONAudioProcessor& p
)
    : AudioProcessorEditor(&p),
      processor(p)
{
    setLookAndFeel(&jadeLook);
    setSize(1120, 760);

    titleLabel.setText(
        "JADE DRAGON",
        juce::dontSendNotification
    );

    titleLabel.setJustificationType(
        juce::Justification::centred
    );

    titleLabel.setFont(
        juce::Font(juce::FontOptions(36.0f, juce::Font::bold))
    );

    titleLabel.setColour(
        juce::Label::textColourId,
        paleGold
    );

    addAndMakeVisible(titleLabel);

    subtitleLabel.setText(
        "PREMIUM CHINESE INSTRUMENT ENGINE",
        juce::dontSendNotification
    );

    subtitleLabel.setJustificationType(
        juce::Justification::centred
    );

    subtitleLabel.setFont(
        juce::Font(juce::FontOptions(11.0f))
    );

    subtitleLabel.setColour(
        juce::Label::textColourId,
        lightJade
    );

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
    setupSlider(reverb, "REVERB");
    setupSlider(delay, "DELAY");
    setupSlider(master, "MASTER");

    auto& state = processor.parameters;

    instrumentAttachment =
        std::make_unique<ComboAttachment>(
            state, "instrument", instrumentSelector
        );

    attackAttachment = std::make_unique<SliderAttachment>(
        state, "attack", attack
    );

    decayAttachment = std::make_unique<SliderAttachment>(
        state, "decay", decay
    );

    sustainAttachment = std::make_unique<SliderAttachment>(
        state, "sustain", sustain
    );

    releaseAttachment = std::make_unique<SliderAttachment>(
        state, "release", release
    );

    brightnessAttachment = std::make_unique<SliderAttachment>(
        state, "brightness", brightness
    );

    driveAttachment = std::make_unique<SliderAttachment>(
        state, "drive", drive
    );

    reverbAttachment = std::make_unique<SliderAttachment>(
        state, "reverb", reverb
    );

    delayAttachment = std::make_unique<SliderAttachment>(
        state, "delay", delay
    );

    masterAttachment = std::make_unique<SliderAttachment>(
        state, "master", master
    );

    startTimerHz(30);
}

JADEDRAGONAudioProcessorEditor::~JADEDRAGONAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void JADEDRAGONAudioProcessorEditor::setupSlider(
    juce::Slider& slider,
    const juce::String& name
)
{
    slider.setName(name);

    slider.setSliderStyle(
        juce::Slider::RotaryHorizontalVerticalDrag
    );

    slider.setTextBoxStyle(
        juce::Slider::TextBoxBelow,
        false,
        76,
        20
    );

    slider.setNumDecimalPlacesToDisplay(2);

    addAndMakeVisible(slider);
}

void JADEDRAGONAudioProcessorEditor::drawDragon(
    juce::Graphics& g,
    juce::Rectangle<float> area
)
{
    const float cx = area.getCentreX();
    const float cy = area.getCentreY();

    const float radius = area.getWidth() * 0.43f;

    // Rotating outer ring.
    {
        juce::Graphics::ScopedSaveState state(g);

        g.addTransform(
            juce::AffineTransform::rotation(
                rotation,
                cx,
                cy
            )
        );

        g.setColour(gold.withAlpha(0.85f));

        g.drawEllipse(
            cx - radius,
            cy - radius,
            radius * 2.0f,
            radius * 2.0f,
            2.0f
        );

        for (int i = 0; i < 24; ++i)
        {
            const float a =
                juce::MathConstants<float>::twoPi *
                static_cast<float>(i) / 24.0f;

            const float x1 = cx + std::cos(a) * (radius - 5.0f);
            const float y1 = cy + std::sin(a) * (radius - 5.0f);

            const float x2 = cx + std::cos(a) * (radius + 5.0f);
            const float y2 = cy + std::sin(a) * (radius + 5.0f);

            g.drawLine(x1, y1, x2, y2, 1.4f);
        }
    }

    // Jade medallion.
    juce::ColourGradient jadeGradient(
        juce::Colour(0xff42c99a),
        cx - radius * 0.7f,
        cy - radius * 0.8f,
        juce::Colour(0xff041a16),
        cx + radius * 0.7f,
        cy + radius * 0.8f,
        false
    );

    g.setGradientFill(jadeGradient);

    g.fillEllipse(
        cx - radius * 0.83f,
        cy - radius * 0.83f,
        radius * 1.66f,
        radius * 1.66f
    );

    g.setColour(paleGold.withAlpha(0.7f));

    g.drawEllipse(
        cx - radius * 0.83f,
        cy - radius * 0.83f,
        radius * 1.66f,
        radius * 1.66f,
        2.0f
    );

    // Dragon silhouette — original decorative path.
    juce::Path dragon;

    dragon.startNewSubPath(
        cx - radius * 0.48f,
        cy + radius * 0.30f
    );

    dragon.cubicTo(
        cx - radius * 0.90f,
        cy - radius * 0.30f,
        cx - radius * 0.10f,
        cy - radius * 1.00f,
        cx + radius * 0.42f,
        cy - radius * 0.50f
    );

    dragon.cubicTo(
        cx + radius * 0.92f,
        cy - radius * 0.08f,
        cx + radius * 0.20f,
        cy + radius * 0.12f,
        cx + radius * 0.42f,
        cy + radius * 0.54f
    );

    dragon.cubicTo(
        cx + radius * 0.05f,
        cy + radius * 0.90f,
        cx - radius * 0.62f,
        cy + radius * 0.62f,
        cx - radius * 0.48f,
        cy + radius * 0.30f
    );

    dragon.closeSubPath();

    // Gold dragon body.
    g.setColour(gold);

    g.strokePath(
        dragon,
        juce::PathStrokeType(
            5.0f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    // Dragon whiskers.
    juce::Path whiskers;

    whiskers.startNewSubPath(
        cx + radius * 0.28f,
        cy - radius * 0.48f
    );

    whiskers.cubicTo(
        cx + radius * 0.75f,
        cy - radius * 0.78f,
        cx + radius * 0.75f,
        cy - radius * 0.18f,
        cx + radius * 0.48f,
        cy - radius * 0.12f
    );

    whiskers.startNewSubPath(
        cx + radius * 0.30f,
        cy - radius * 0.42f
    );

    whiskers.cubicTo(
        cx + radius * 0.85f,
        cy - radius * 0.45f,
        cx + radius * 0.80f,
        cy + radius * 0.05f,
        cx + radius * 0.43f,
        cy + radius * 0.04f
    );

    g.setColour(paleGold);

    g.strokePath(
        whiskers,
        juce::PathStrokeType(
            2.0f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    // Dragon eye.
    g.setColour(juce::Colour(0xffff5b37));

    g.fillEllipse(
        cx + radius * 0.36f,
        cy - radius * 0.48f,
        7.0f,
        7.0f
    );

    // Small central jewel.
    g.setColour(paleGold);

    g.fillEllipse(
        cx - 5.0f,
        cy - 5.0f,
        10.0f,
        10.0f
    );
}

void JADEDRAGONAudioProcessorEditor::paint(
    juce::Graphics& g
)
{
    auto bounds = getLocalBounds().toFloat();

    // Deep jade background.
    g.setGradientFill(
        juce::ColourGradient(
            juce::Colour(0xff143c30),
            0.0f,
            0.0f,
            black,
            0.0f,
            static_cast<float>(getHeight()),
            false
        )
    );

    g.fillAll();

    // Subtle architectural grid.
    g.setColour(lightJade.withAlpha(0.035f));

    for (int x = 0; x < getWidth(); x += 36)
        g.drawVerticalLine(x, 0.0f, static_cast<float>(getHeight()));

    for (int y = 0; y < getHeight(); y += 36)
        g.drawHorizontalLine(y, 0.0f, static_cast<float>(getWidth()));

    // Outer metallic frame.
    g.setColour(gold.withAlpha(0.85f));

    g.drawRoundedRectangle(
        bounds.reduced(9.0f),
        18.0f,
        2.0f
    );

    g.setColour(paleGold.withAlpha(0.18f));

    g.drawRoundedRectangle(
        bounds.reduced(15.0f),
        15.0f,
        1.0f
    );

    // Decorative corner details.
    const float corner = 38.0f;

    g.setColour(gold);

    for (int sx : { 1, -1 })
    {
        for (int sy : { 1, -1 })
        {
            const float x = sx > 0 ? 20.0f : getWidth() - 20.0f;
            const float y = sy > 0 ? 20.0f : getHeight() - 20.0f;

            g.drawLine(
                x,
                y,
                x + sx * corner,
                y,
                2.0f
            );

            g.drawLine(
                x,
                y,
                x,
                y + sy * corner,
                2.0f
            );
        }
    }

    // Header.
    titleLabel.setBounds(0, 20, getWidth(), 48);
    subtitleLabel.setBounds(0, 67, getWidth(), 18);

    // Section backgrounds.
    g.setColour(deepJade.withAlpha(0.88f));

    g.fillRoundedRectangle(
        24.0f, 270.0f,
        static_cast<float>(getWidth()) - 48.0f,
        205.0f,
        15.0f
    );

    g.fillRoundedRectangle(
        24.0f, 500.0f,
        static_cast<float>(getWidth()) - 48.0f,
        210.0f,
        15.0f
    );

    g.setColour(gold.withAlpha(0.4f));

    g.drawRoundedRectangle(
        24.0f, 270.0f,
        static_cast<float>(getWidth()) - 48.0f,
        205.0f,
        15.0f,
        1.0f
    );

    g.drawRoundedRectangle(
        24.0f, 500.0f,
        static_cast<float>(getWidth()) - 48.0f,
        210.0f,
        15.0f,
        1.0f
    );

    // Dragon medallion.
    drawDragon(
        g,
        juce::Rectangle<float>(
            getWidth() * 0.5f - 100.0f,
            90.0f,
            200.0f,
            170.0f
        )
    );

    // Section captions.
    g.setColour(paleGold);
    g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));

    g.drawText(
        "01  |  INSTRUMENT CHARACTER",
        45, 280, 400, 20,
        juce::Justification::left
    );

    g.drawText(
        "02  |  SOUND DESIGN & EFFECTS",
        45, 510, 400, 20,
        juce::Justification::left
    );

    // Footer.
    g.setColour(lightJade.withAlpha(0.7f));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));

    g.drawText(
        "HUNTERICE RECORDS     •     JADE DRAGON V2",
        0,
        getHeight() - 25,
        getWidth(),
        15,
        juce::Justification::centred
    );
}

void JADEDRAGONAudioProcessorEditor::resized()
{
    const int w = getWidth();

    instrumentSelector.setBounds(
        w / 2 - 100,
        230,
        200,
        29
    );

    const int count = 5;
    const int sliderWidth = 175;
    const int sliderHeight = 150;
    const int gap = 25;

    const int totalWidth =
        count * sliderWidth + (count - 1) * gap;

    const int startX = (w - totalWidth) / 2;

    juce::Slider* top[] =
    {
        &attack,
        &decay,
        &sustain,
        &release,
        &brightness
    };

    juce::Slider* bottom[] =
    {
        &drive,
        &reverb,
        &delay,
        &master
    };

    for (int i = 0; i < count; ++i)
    {
        top[i]->setBounds(
            startX + i * (sliderWidth + gap),
            310,
            sliderWidth,
            sliderHeight
        );
    }

    const int bottomCount = 4;
    const int bottomTotal =
        bottomCount * sliderWidth + (bottomCount - 1) * gap;

    const int bottomStart = (w - bottomTotal) / 2;

    for (int i = 0; i < bottomCount; ++i)
    {
        bottom[i]->setBounds(
            bottomStart + i * (sliderWidth + gap),
            535,
            sliderWidth,
            sliderHeight
        );
    }
}

void JADEDRAGONAudioProcessorEditor::timerCallback()
{
    rotation += 0.0025f;

    if (rotation >= juce::MathConstants<float>::twoPi)
        rotation -= juce::MathConstants<float>::twoPi;

    glow += 0.025f;

    if (glow >= juce::MathConstants<float>::twoPi)
        glow = 0.0f;

    repaint();
}