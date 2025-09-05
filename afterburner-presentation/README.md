# ESP8266 Afterburner Controller - Presentation Website

A modern, responsive presentation website for the ESP8266 Afterburner Controller project. This website showcases the product features, demos, and provides download links for the mobile app and firmware.

## 🚀 Features

- **Modern Design**: Clean, professional design with smooth animations
- **Responsive Layout**: Works perfectly on desktop, tablet, and mobile devices
- **Interactive Demo Section**: Showcase videos and images of the product in action
- **Download Center**: Easy access to Android/iOS apps and source code
- **Feature Showcase**: Comprehensive overview of all product capabilities
- **SEO Optimized**: Proper meta tags and social media integration

## 🛠️ Technology Stack

- **React 18** with TypeScript
- **Framer Motion** for smooth animations
- **React Icons** for consistent iconography
- **CSS3** with modern features (Grid, Flexbox, Custom Properties)
- **Responsive Design** with mobile-first approach

## 📦 Installation

1. **Clone the repository**:

   ```bash
   git clone <repository-url>
   cd afterburner-presentation
   ```

2. **Install dependencies**:

   ```bash
   npm install
   ```

3. **Start development server**:

   ```bash
   npm start
   ```

4. **Open your browser** and navigate to `http://localhost:3000`

## 🏗️ Project Structure

```
src/
├── components/
│   ├── Hero.tsx              # Hero section with product introduction
│   ├── Features.tsx          # Product features showcase
│   ├── Demo.tsx              # Demo videos and images
│   ├── Downloads.tsx         # Download links and installation guide
│   └── Footer.tsx            # Footer with links and social media
├── App.tsx                   # Main application component
├── App.css                   # Global styles and utilities
└── index.tsx                 # Application entry point
```

## 🎨 Design Features

### Hero Section

- Animated background with stars and nebula effects
- Interactive mobile app mockup
- Call-to-action buttons
- Feature highlights

### Features Section

- Grid layout showcasing 9 key features
- Hover animations and effects
- Reliability statistics
- Color-coded feature icons

### Demo Section

- Interactive video player
- Thumbnail gallery
- Multiple demo categories
- Download and source links

### Downloads Section

- Platform selector (Android/iOS/Source)
- Release information with changelog
- Installation guide
- GitHub integration

### Footer

- Comprehensive link organization
- Social media integration
- Technology stack showcase
- Animated background effects

## 📱 Responsive Design

The website is fully responsive and optimized for:

- **Desktop**: 1200px+ (Full layout with sidebars)
- **Tablet**: 768px - 1199px (Adjusted grid layouts)
- **Mobile**: 320px - 767px (Stacked layout, touch-friendly)

## 🎭 Animations

Built with Framer Motion for smooth, performant animations:

- **Page Load**: Staggered component animations
- **Scroll Triggers**: Elements animate into view
- **Hover Effects**: Interactive feedback
- **Background**: Animated stars and nebula effects

## 🔧 Customization

### Colors

The website uses CSS custom properties for easy theming:

```css
:root {
  --primary-color: #4ecdc4;
  --secondary-color: #ff6b6b;
  --accent-color: #45b7d1;
}
```

### Content

- Update product information in component files
- Replace placeholder images with actual product photos
- Modify download links to point to your GitHub releases
- Customize social media links in Footer component

## 📈 SEO & Performance

- **Meta Tags**: Comprehensive SEO meta tags
- **Open Graph**: Social media sharing optimization
- **Performance**: Optimized images and lazy loading
- **Accessibility**: Proper ARIA labels and semantic HTML

## 🚀 Deployment

### Build for Production

```bash
npm run build
```

### Deploy to GitHub Pages

```bash
npm install --save-dev gh-pages
npm run build
npx gh-pages -d build
```

### Deploy to Netlify

1. Connect your GitHub repository
2. Set build command: `npm run build`
3. Set publish directory: `build`

## 📝 Content Updates

### Adding New Features

1. Edit `src/components/Features.tsx`
2. Add new feature object to the features array
3. Include appropriate icon and description

### Adding New Demos

1. Edit `src/components/Demo.tsx`
2. Add new demo object to the demos array
3. Include thumbnail, description, and media type

### Updating Downloads

1. Edit `src/components/Downloads.tsx`
2. Update the releases array with new version information
3. Update file URLs to point to actual GitHub releases

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Framer Motion** for smooth animations
- **React Icons** for consistent iconography
- **Google Fonts** for typography
- **ESP8266 Afterburner Team** for the amazing product

---

**ESP8266 Afterburner Controller Presentation Website** - Showcasing the future of RC LED effects! 🛩️✨
