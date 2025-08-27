# Android App Icons - ESP32 C3 Afterburner

This document describes the icon set created for the ESP32 C3 Afterburner Android application.

## 🎨 Icon Design Concept

The icon design represents the core functionality of the ESP32 C3 Afterburner system:

- **Engine Core**: Orange/red circular element representing the jet engine
- **Afterburner Flame**: Dynamic flame effect extending from the core
- **Sparkle Effects**: Golden sparkles representing LED effects
- **LED Strip**: Green elements representing the WS2812B LED strip
- **ESP32 Module**: Blue square representing the ESP32 C3 controller
- **Color Scheme**: Afterburner-themed orange, red, and gold colors

## 📱 Icon Files Created

### 1. Adaptive Icons (Android 8.0+)

- **ic_launcher.xml** - Main adaptive icon configuration
- **ic_launcher_round.xml** - Round adaptive icon configuration
- **ic_launcher_foreground.xml** - Foreground vector drawable
- **ic_launcher_background.xml** - Background vector drawable

### 2. Notification Icon

- **ic_notification.xml** - 24dp notification icon

### 3. Splash Screen

- **splash_icon.xml** - Large splash screen icon (200dp)
- **launch_screen.xml** - Splash screen layout
- **SplashActivity.kt** - Splash screen activity

### 4. Color Resources

- **colors.xml** - App-specific color definitions
- **styles.xml** - Splash screen theme

## 🎯 Icon Specifications

### Adaptive Icon

- **Size**: 108dp x 108dp
- **Foreground**: Engine core with flame effects
- **Background**: Radial gradient in afterburner colors
- **Safe Zone**: 66dp x 66dp (center area)

### Notification Icon

- **Size**: 24dp x 24dp
- **Style**: Simplified engine core with sparkles
- **Colors**: Orange core with gold sparkles

### Splash Screen Icon

- **Size**: 200dp x 200dp
- **Style**: Detailed representation with text elements
- **Background**: Dark theme with afterburner colors

## 🌈 Color Palette

### Primary Colors

- **Afterburner Primary**: #FF4500 (Orange Red)
- **Afterburner Secondary**: #FF6B35 (Orange)
- **Afterburner Accent**: #FFD700 (Gold)
- **Afterburner Dark**: #1A1A1A (Dark Gray)
- **Afterburner Light**: #FF6347 (Tomato)

### Splash Screen Colors

- **Background**: #1A1A1A (Dark Gray)
- **Text Primary**: #FFFFFF (White)
- **Text Secondary**: #CCCCCC (Light Gray)

## 📐 Technical Details

### Vector Drawables

All icons are created as vector drawables (XML) for:

- **Scalability**: Perfect rendering at any size
- **File Size**: Small file sizes compared to bitmap images
- **Maintenance**: Easy to modify and update
- **Performance**: Fast rendering on all devices

### Adaptive Icon Support

- **Android 8.0+**: Full adaptive icon support
- **Legacy Support**: Fallback to traditional icons
- **Shape Adaptation**: Works with all launcher shapes

### Splash Screen Implementation

- **Activity-based**: Uses dedicated SplashActivity
- **Theme Integration**: Custom splash theme
- **Timing**: 2-second display duration
- **Smooth Transition**: Seamless transition to main app

## 🔧 Implementation

### AndroidManifest.xml Changes

```xml
<!-- Splash Screen Activity -->
<activity
  android:name=".SplashActivity"
  android:theme="@style/SplashTheme"
  android:exported="true">
  <intent-filter>
      <action android:name="android.intent.action.MAIN" />
      <category android:name="android.intent.category.LAUNCHER" />
  </intent-filter>
</activity>
```

### SplashActivity.kt

```kotlin
class SplashActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.launch_screen)

        Handler(Looper.getMainLooper()).postDelayed({
            val intent = Intent(this, MainActivity::class.java)
            startActivity(intent)
            finish()
        }, 2000)
    }
}
```

## 📱 Device Compatibility

### Icon Support

- **Android 8.0+**: Full adaptive icon support
- **Android 7.1 and below**: Traditional icon support
- **All Densities**: MDPI, HDPI, XHDPI, XXHDPI, XXXHDPI

### Splash Screen Support

- **Android 5.0+**: Full splash screen support
- **All Screen Sizes**: Responsive design
- **Dark/Light Themes**: Theme-aware colors

## 🎨 Design Principles

### Visual Hierarchy

1. **Engine Core**: Primary focal point
2. **Flame Effects**: Secondary visual element
3. **Sparkle Effects**: Accent details
4. **ESP32 Module**: Technical representation

### Color Psychology

- **Orange/Red**: Energy, power, heat (afterburner theme)
- **Gold**: Premium, high-tech, sparkle effects
- **Blue**: Technology, reliability (ESP32)
- **Green**: LED strips, electronics

### Brand Consistency

- **Consistent Colors**: Same palette across all icons
- **Unified Style**: Vector-based design language
- **Scalable Design**: Works at all sizes
- **Theme Integration**: Adapts to system themes

## 🔄 Future Enhancements

### Potential Improvements

- **Animated Icons**: Dynamic flame effects
- **Dark/Light Variants**: Theme-specific icons
- **Custom Launcher**: Specialized launcher integration
- **Widget Icons**: Home screen widget support

### Icon Variations

- **Monochrome**: Single-color versions
- **Grayscale**: Black and white variants
- **Inverted**: Light background versions
- **Minimalist**: Simplified versions

## 📋 Maintenance

### Icon Updates

1. **Vector Files**: Modify XML drawables
2. **Colors**: Update colors.xml
3. **Testing**: Verify on different devices
4. **Documentation**: Update this file

### Best Practices

- **Vector First**: Always use vector drawables
- **Test Multiple Sizes**: Verify at all densities
- **Accessibility**: Ensure good contrast
- **Performance**: Keep file sizes small

---

**Icon Set Version**: 1.0  
**Created**: 2024  
**Design**: ESP32 C3 Afterburner Theme  
**Format**: Vector Drawables (XML)
