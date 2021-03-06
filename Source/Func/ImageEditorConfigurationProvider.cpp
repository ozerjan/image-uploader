#include "ImageEditorConfigurationProvider.h"

#include "Core/Settings/WtlGuiSettings.h"

ImageEditorConfigurationProvider::ImageEditorConfigurationProvider()
{
    WtlGuiSettings& Settings = *ServiceLocator::instance()->settings<WtlGuiSettings>();
    penSize_ = Settings.ImageEditorSettings.PenSize;
    foregroundColor_ = Settings.ImageEditorSettings.ForegroundColor;
    backgroundColor_ = Settings.ImageEditorSettings.BackgroundColor;
    stepForegroundColor_ = Settings.ImageEditorSettings.StepForegroundColor;
    stepBackgroundColor_ = Settings.ImageEditorSettings.StepBackgroundColor;
    font_ =  Settings.ImageEditorSettings.Font;
    roundingRadius_ = Settings.ImageEditorSettings.RoundingRadius;
    allowAltTab_ = Settings.ImageEditorSettings.AllowAltTab;
    searchEngine_ = Settings.ImageEditorSettings.SearchEngine;
    fillTextBackground_ = Settings.ImageEditorSettings.FillTextBackground;
    arrowMode_ = Settings.ImageEditorSettings.ArrowType;
}

void ImageEditorConfigurationProvider::saveConfiguration()
{
    WtlGuiSettings& Settings = *ServiceLocator::instance()->settings<WtlGuiSettings>();
    Settings.ImageEditorSettings.PenSize = penSize_;
    Settings.ImageEditorSettings.ForegroundColor = foregroundColor_;
    Settings.ImageEditorSettings.BackgroundColor = backgroundColor_;
    Settings.ImageEditorSettings.Font = font_;
    Settings.ImageEditorSettings.RoundingRadius = roundingRadius_;
    Settings.ImageEditorSettings.AllowAltTab = allowAltTab_;
    Settings.ImageEditorSettings.SearchEngine = searchEngine_;
    Settings.ImageEditorSettings.FillTextBackground = fillTextBackground_;
    Settings.ImageEditorSettings.StepForegroundColor = stepForegroundColor_;
    Settings.ImageEditorSettings.StepBackgroundColor = stepBackgroundColor_;
    Settings.ImageEditorSettings.ArrowType = arrowMode_;
}

