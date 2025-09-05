import React from 'react';
import { motion } from 'framer-motion';
import { 
  FaGithub, 
  FaTwitter, 
  FaDiscord, 
  FaEnvelope, 
  FaHeart,
  FaRocket,
  FaCode,
  FaWifi,
  FaMobile
} from 'react-icons/fa';
import './Footer.css';

const Footer: React.FC = () => {
  const currentYear = new Date().getFullYear();

  const socialLinks = [
    {
      name: 'GitHub',
      icon: <span>📁</span>,
      url: 'https://github.com/your-repo',
      color: '#333'
    },
    {
      name: 'Twitter',
      icon: <span>🐦</span>,
      url: 'https://twitter.com/your-handle',
      color: '#1DA1F2'
    },
    {
      name: 'Discord',
      icon: <span>💬</span>,
      url: 'https://discord.gg/your-server',
      color: '#5865F2'
    },
    {
      name: 'Email',
      icon: <span>📧</span>,
      url: 'mailto:contact@example.com',
      color: '#EA4335'
    }
  ];

  const quickLinks = [
    { name: 'Features', href: '#features' },
    { name: 'Demo', href: '#demo' },
    { name: 'Downloads', href: '#downloads' },
    { name: 'Documentation', href: 'https://github.com/your-repo' },
    { name: 'Troubleshooting', href: 'https://github.com/your-repo' },
    { name: 'Contributing', href: 'https://github.com/your-repo' }
  ];

  const techStack = [
    { name: 'ESP8266', icon: <span>📶</span> },
    { name: 'React Native', icon: <span>📱</span> },
    { name: 'WebSocket', icon: <span>💻</span> },
    { name: 'FastLED', icon: <span>🚀</span> }
  ];

  return (
    <footer className="footer">
      <div className="container">
        <motion.div 
          className="footer-content"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8 }}
          viewport={{ once: true }}
        >
          <div className="footer-main">
            <div className="footer-brand">
              <div className="brand-logo">
                <span>🚀</span>
                <span>ESP8266 Afterburner</span>
              </div>
              <p className="brand-description">
                A sophisticated LED afterburner controller for RC models featuring automatic 
                throttle calibration, real-time LED effects, and wireless control.
              </p>
              <div className="tech-stack">
                <h4>Built With:</h4>
                <div className="tech-items">
                  {techStack.map((tech, index) => (
                    <div key={index} className="tech-item">
                      <span className="tech-icon">{tech.icon}</span>
                      <span className="tech-name">{tech.name}</span>
                    </div>
                  ))}
                </div>
              </div>
            </div>

            <div className="footer-links">
              <div className="link-section">
                <h4>Quick Links</h4>
                <ul>
                  {quickLinks.map((link, index) => (
                    <li key={index}>
                      <a href={link.href}>{link.name}</a>
                    </li>
                  ))}
                </ul>
              </div>

              <div className="link-section">
                <h4>Resources</h4>
                <ul>
                  <li><a href="https://github.com/your-repo">Source Code</a></li>
                  <li><a href="https://github.com/your-repo/releases">Releases</a></li>
                  <li><a href="https://github.com/your-repo/issues">Issues</a></li>
                  <li><a href="https://github.com/your-repo/wiki">Wiki</a></li>
                  <li><a href="https://github.com/your-repo/discussions">Discussions</a></li>
                  <li><a href="https://github.com/your-repo/blob/main/LICENSE">License</a></li>
                </ul>
              </div>

              <div className="link-section">
                <h4>Community</h4>
                <ul>
                  <li><a href="https://github.com/your-repo">GitHub</a></li>
                  <li><a href="https://discord.gg/your-server">Discord</a></li>
                  <li><a href="https://twitter.com/your-handle">Twitter</a></li>
                  <li><a href="mailto:contact@example.com">Contact</a></li>
                </ul>
              </div>
            </div>
          </div>

          <div className="footer-bottom">
            <div className="footer-bottom-content">
              <div className="copyright">
                <p>
                  © {currentYear} ESP8266 Afterburner Controller. Made with{' '}
                  <span className="heart-icon">❤️</span> by the community.
                </p>
                <p className="license">
                  Licensed under the{' '}
                  <a href="https://github.com/your-repo/blob/main/LICENSE">MIT License</a>
                </p>
              </div>

              <div className="social-links">
                {socialLinks.map((social, index) => (
                  <motion.a
                    key={index}
                    href={social.url}
                    className="social-link"
                    style={{ '--social-color': social.color } as React.CSSProperties}
                    whileHover={{ scale: 1.1, y: -2 }}
                    whileTap={{ scale: 0.95 }}
                    target="_blank"
                    rel="noopener noreferrer"
                    aria-label={social.name}
                  >
                    {social.icon}
                  </motion.a>
                ))}
              </div>
            </div>
          </div>
        </motion.div>
      </div>

      <div className="footer-background">
        <div className="footer-stars"></div>
        <div className="footer-nebula"></div>
      </div>
    </footer>
  );
};

export default Footer;
