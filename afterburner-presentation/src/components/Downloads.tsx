import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { 
  FaAndroid, 
  FaApple, 
  FaGithub, 
  FaDownload, 
  FaCode, 
  FaFileArchive,
  FaExternalLinkAlt,
  FaCheckCircle,
  FaClock,
  FaTag
} from 'react-icons/fa';
import './Downloads.css';

const Downloads: React.FC = () => {
  const [selectedPlatform, setSelectedPlatform] = useState('android');

  const platforms = {
    android: {
      name: 'Android',
      icon: <span>🤖</span>,
      color: '#3DDC84',
      description: 'React Native app for Android devices'
    },
    ios: {
      name: 'iOS',
      icon: <span>🍎</span>,
      color: '#007AFF',
      description: 'React Native app for iOS devices'
    },
    source: {
      name: 'Source Code',
      icon: <span>💻</span>,
      color: '#FF6B6B',
      description: 'Complete source code and firmware'
    }
  };

  const releases = [
    {
      version: 'v2.1.0',
      date: '2024-01-15',
      type: 'stable',
      size: '15.2 MB',
      downloads: 1250,
      features: [
        'Crash prevention mechanisms',
        'Optimized WebSocket communication',
        'Enhanced color picker',
        'Improved error handling',
        'Streamlined UI'
      ],
      files: {
        android: {
          url: 'https://github.com/your-repo/releases/download/v2.1.0/AfterburnerControl-v2.1.0.apk',
          filename: 'AfterburnerControl-v2.1.0.apk',
          size: '15.2 MB'
        },
        ios: {
          url: 'https://github.com/your-repo/releases/download/v2.1.0/AfterburnerControl-v2.1.0.ipa',
          filename: 'AfterburnerControl-v2.1.0.ipa',
          size: '18.7 MB'
        },
        source: {
          url: 'https://github.com/your-repo/archive/refs/tags/v2.1.0.zip',
          filename: 'afterburner-v2.1.0.zip',
          size: '2.3 MB'
        }
      }
    },
    {
      version: 'v2.0.0',
      date: '2023-12-01',
      type: 'stable',
      size: '14.8 MB',
      downloads: 890,
      features: [
        'Automatic throttle calibration',
        'Enhanced debugging',
        'Improved PWM signal handling',
        'NodeMCU V3 optimization'
      ],
      files: {
        android: {
          url: 'https://github.com/your-repo/releases/download/v2.0.0/AfterburnerControl-v2.0.0.apk',
          filename: 'AfterburnerControl-v2.0.0.apk',
          size: '14.8 MB'
        },
        ios: {
          url: 'https://github.com/your-repo/releases/download/v2.0.0/AfterburnerControl-v2.0.0.ipa',
          filename: 'AfterburnerControl-v2.0.0.ipa',
          size: '17.9 MB'
        },
        source: {
          url: 'https://github.com/your-repo/archive/refs/tags/v2.0.0.zip',
          filename: 'afterburner-v2.0.0.zip',
          size: '2.1 MB'
        }
      }
    }
  ];

  const handleDownload = (url: string, filename: string) => {
    // In a real implementation, this would trigger the download
    console.log(`Downloading ${filename} from ${url}`);
    // For demo purposes, we'll just show an alert
    alert(`Download would start: ${filename}`);
  };

  return (
    <section id="downloads" className="downloads section">
      <div className="container">
        <motion.div 
          className="downloads-header"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8 }}
          viewport={{ once: true }}
        >
          <h2 className="section-title">
            <span className="gradient-text">Download & Install</span>
          </h2>
          <p className="section-description">
            Get the latest version of the Afterburner Control app and firmware
          </p>
        </motion.div>

        <motion.div 
          className="platform-selector"
          initial={{ opacity: 0, y: 20 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.2 }}
          viewport={{ once: true }}
        >
          {Object.entries(platforms).map(([key, platform]) => (
            <button
              key={key}
              className={`platform-btn ${selectedPlatform === key ? 'active' : ''}`}
              onClick={() => setSelectedPlatform(key)}
              style={{ '--platform-color': platform.color } as React.CSSProperties}
            >
              <div className="platform-icon">{platform.icon}</div>
              <div className="platform-info">
                <h4>{platform.name}</h4>
                <p>{platform.description}</p>
              </div>
            </button>
          ))}
        </motion.div>

        <motion.div 
          className="releases-container"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.4 }}
          viewport={{ once: true }}
        >
          {releases.map((release, index) => (
            <motion.div
              key={release.version}
              className="release-card card"
              initial={{ opacity: 0, x: -30 }}
              whileInView={{ opacity: 1, x: 0 }}
              transition={{ duration: 0.6, delay: index * 0.1 }}
              viewport={{ once: true }}
            >
              <div className="release-header">
                <div className="release-info">
                  <div className="version-badge">
                    <span>🏷</span>
                    <span>{release.version}</span>
                  </div>
                  <div className="release-meta">
                    <span className="release-date">
                      <span>🕐</span>
                      {release.date}
                    </span>
                    <span className="release-size">
                      <span>📦</span>
                      {release.size}
                    </span>
                    <span className="release-downloads">
                      <span>⬇</span>
                      {release.downloads.toLocaleString()} downloads
                    </span>
                  </div>
                </div>
                <div className="release-type">
                  <span className={`type-badge ${release.type}`}>
                    {release.type === 'stable' ? <span>✅</span> : <span>🕐</span>}
                    {release.type}
                  </span>
                </div>
              </div>

              <div className="release-features">
                <h4>What's New:</h4>
                <ul>
                  {release.features.map((feature, featureIndex) => (
                    <li key={featureIndex}>
                      <span>✅</span>
                      {feature}
                    </li>
                  ))}
                </ul>
              </div>

              <div className="release-download">
                <div className="download-info">
                  <h4>Download for {platforms[selectedPlatform as keyof typeof platforms].name}</h4>
                  <p>
                    <strong>File:</strong> {release.files[selectedPlatform as keyof typeof release.files].filename}
                    <br />
                    <strong>Size:</strong> {release.files[selectedPlatform as keyof typeof release.files].size}
                  </p>
                </div>
                <div className="download-actions">
                  <button
                    className="btn btn-primary download-btn"
                    onClick={() => handleDownload(
                      release.files[selectedPlatform as keyof typeof release.files].url,
                      release.files[selectedPlatform as keyof typeof release.files].filename
                    )}
                  >
                    <span>⬇</span>
                    Download {platforms[selectedPlatform as keyof typeof platforms].name}
                  </button>
                  <a
                    href={release.files[selectedPlatform as keyof typeof release.files].url}
                    className="btn btn-secondary"
                    target="_blank"
                    rel="noopener noreferrer"
                  >
                    <span>🔗</span>
                    View on GitHub
                  </a>
                </div>
              </div>
            </motion.div>
          ))}
        </motion.div>

        <motion.div 
          className="installation-guide"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.6 }}
          viewport={{ once: true }}
        >
          <div className="guide-card card">
            <h3>Installation Guide</h3>
            <div className="guide-steps">
              <div className="step">
                <div className="step-number">1</div>
                <div className="step-content">
                  <h4>Download the App</h4>
                  <p>Download the APK file for Android or IPA for iOS from the latest release above.</p>
                </div>
              </div>
              <div className="step">
                <div className="step-number">2</div>
                <div className="step-content">
                  <h4>Install on Device</h4>
                  <p>For Android: Enable "Install from unknown sources" and install the APK. For iOS: Use Xcode or TestFlight.</p>
                </div>
              </div>
              <div className="step">
                <div className="step-number">3</div>
                <div className="step-content">
                  <h4>Connect to ESP8266</h4>
                  <p>Connect to the "Afterburner_AP" WiFi network and launch the app to start controlling your LEDs.</p>
                </div>
              </div>
            </div>
            <div className="guide-actions">
              <a href="https://github.com/your-repo" className="btn btn-secondary">
                <span>📁</span>
                View Documentation
              </a>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Downloads;
