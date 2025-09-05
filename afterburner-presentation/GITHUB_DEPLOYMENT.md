# GitHub Pages Deployment Guide

This guide will help you deploy the ESP8266 Afterburner Controller presentation website to GitHub Pages.

## 🚀 Quick Setup

### Step 1: Create GitHub Repository

1. Go to [GitHub.com](https://github.com) and sign in
2. Click the "+" icon in the top right corner
3. Select "New repository"
4. Name your repository: `afterburner-presentation`
5. Make it **Public** (required for free GitHub Pages)
6. **Don't** initialize with README, .gitignore, or license (we already have these)
7. Click "Create repository"

### Step 2: Update Package.json

**IMPORTANT**: Replace `yourusername` with your actual GitHub username in the `homepage` field:

```json
"homepage": "https://yourusername.github.io/afterburner-presentation"
```

For example, if your GitHub username is `johndoe`:

```json
"homepage": "https://johndoe.github.io/afterburner-presentation"
```

### Step 3: Initialize Git and Push to GitHub

Run these commands in your project directory:

```bash
# Initialize git repository
git init

# Add all files
git add .

# Make initial commit
git commit -m "Initial commit: ESP8266 Afterburner presentation website"

# Add your GitHub repository as remote (replace YOUR_USERNAME)
git remote add origin https://github.com/YOUR_USERNAME/afterburner-presentation.git

# Push to GitHub
git push -u origin main
```

### Step 4: Deploy to GitHub Pages

```bash
# Deploy to GitHub Pages
npm run deploy
```

This command will:

1. Build the production version of your site
2. Create a `gh-pages` branch
3. Push the built files to GitHub Pages
4. Make your site available at `https://yourusername.github.io/afterburner-presentation`

## 🔧 Alternative: GitHub Actions (Recommended)

For automatic deployment on every push, create `.github/workflows/deploy.yml`:

```yaml
name: Deploy to GitHub Pages

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  build-and-deploy:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v3

      - name: Setup Node.js
        uses: actions/setup-node@v3
        with:
          node-version: "18"
          cache: "npm"

      - name: Install dependencies
        run: npm ci

      - name: Build
        run: npm run build

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v3
        if: github.ref == 'refs/heads/main'
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: ./build
```

## 📝 Custom Domain (Optional)

If you have a custom domain:

1. Create a `CNAME` file in the `public` folder:

   ```
   yourdomain.com
   ```

2. Update your domain's DNS settings:

   - Add a CNAME record pointing to `yourusername.github.io`

3. In GitHub repository settings:
   - Go to Settings → Pages
   - Set Custom domain to `yourdomain.com`
   - Enable "Enforce HTTPS"

## 🔄 Updating Your Site

### Method 1: Manual Deploy

```bash
# Make your changes
# Commit and push to main branch
git add .
git commit -m "Update website content"
git push origin main

# Deploy to GitHub Pages
npm run deploy
```

### Method 2: Automatic Deploy (with GitHub Actions)

```bash
# Just push to main branch - deployment happens automatically
git add .
git commit -m "Update website content"
git push origin main
```

## 🛠️ Troubleshooting

### Common Issues:

1. **404 Error**:

   - Check that your repository is public
   - Verify the homepage URL in package.json matches your GitHub username
   - Wait 5-10 minutes after deployment

2. **Build Fails**:

   - Run `npm run build` locally to check for errors
   - Ensure all dependencies are installed: `npm install`

3. **Styling Issues**:

   - Check that all CSS files are properly imported
   - Verify image paths are correct

4. **GitHub Pages Not Enabled**:
   - Go to repository Settings → Pages
   - Select "Deploy from a branch"
   - Choose "gh-pages" branch and "/ (root)" folder

## 📊 Repository Structure

After deployment, your repository will have:

```
afterburner-presentation/
├── .github/
│   └── workflows/
│       └── deploy.yml          # GitHub Actions (optional)
├── public/
│   ├── index.html
│   └── CNAME                   # Custom domain (optional)
├── src/
│   ├── components/
│   ├── App.tsx
│   └── App.css
├── build/                      # Generated during deployment
├── package.json
└── README.md
```

## 🎯 Next Steps

1. **Customize Content**: Update the website with your actual product information
2. **Add Real Images**: Replace placeholder images with actual product photos
3. **Update Links**: Point download links to your actual GitHub releases
4. **SEO Optimization**: Update meta tags with your specific information
5. **Analytics**: Add Google Analytics or similar tracking

## 🔗 Useful Links

- [GitHub Pages Documentation](https://docs.github.com/en/pages)
- [React Deployment Guide](https://create-react-app.dev/docs/deployment/#github-pages)
- [Custom Domain Setup](https://docs.github.com/en/pages/configuring-a-custom-domain-for-your-github-pages-site)

---

**Your ESP8266 Afterburner Controller presentation website will be live at:**
`https://yourusername.github.io/afterburner-presentation` 🚀
