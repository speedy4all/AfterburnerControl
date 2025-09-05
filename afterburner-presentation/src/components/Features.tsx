import React from 'react';
import { motion } from 'framer-motion';
import { 
  FaWifi, 
  FaMobile, 
  FaCog, 
  FaLightbulb, 
  FaBolt, 
  FaShieldAlt,
  FaMicrochip,
  FaGamepad,
  FaPalette,
  FaSync
} from 'react-icons/fa';
import './Features.css';

const Features: React.FC = () => {
  const features = [
    {
      icon: <span>⚙️</span>,
      title: "Automatic Throttle Calibration",
      description: "Works with any transmitter (Futaba, Spektrum, FlySky, etc.) - no manual configuration needed",
      color: "#ff6b6b"
    },
    {
      icon: <span>💡</span>,
      title: "Real-time LED Effects",
      description: "Dynamic afterburner effects that respond to throttle input with multiple animation modes",
      color: "#4ecdc4"
    },
    {
      icon: <span>📱</span>,
      title: "Wireless Control",
      description: "React Native app for real-time settings adjustment via WiFi connection",
      color: "#45b7d1"
    },
    {
      icon: <span>🎨</span>,
      title: "Multiple Effect Modes",
      description: "Static, Pulse, and Afterburner modes with customizable colors and brightness",
      color: "#96ceb4"
    },
    {
      icon: <span>📶</span>,
      title: "WebSocket Communication",
      description: "Real-time bidirectional communication with optimized reliability",
      color: "#feca57"
    },
    {
      icon: <span>🛡️</span>,
      title: "Crash Prevention",
      description: "Optimized to prevent controller crashes during settings updates",
      color: "#ff9ff3"
    },
    {
      icon: <span>🔧</span>,
      title: "ESP8266 NodeMCU V3",
      description: "Compatible with ESP8266 NodeMCU V3 and supports up to 300 LEDs",
      color: "#54a0ff"
    },
    {
      icon: <span>⚡</span>,
      title: "5V Power Supply",
      description: "Supports 5V power supply for LED strips with proper level shifting",
      color: "#5f27cd"
    },
    {
      icon: <span>🎮</span>,
      title: "PWM Throttle Input",
      description: "Automatic PWM range detection and calibration for any RC transmitter",
      color: "#00d2d3"
    }
  ];

  const containerVariants = {
    hidden: { opacity: 0 },
    visible: {
      opacity: 1,
      transition: {
        staggerChildren: 0.1
      }
    }
  };

  const itemVariants = {
    hidden: { opacity: 0, y: 30 },
    visible: { opacity: 1, y: 0 }
  };

  return (
    <section id="features" className="features section">
      <div className="container">
        <motion.div 
          className="features-header"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8 }}
          viewport={{ once: true }}
        >
          <h2 className="section-title">
            <span className="gradient-text">Powerful Features</span>
          </h2>
          <p className="section-description">
            Everything you need for professional RC afterburner effects
          </p>
        </motion.div>

        <motion.div 
          className="features-grid"
          variants={containerVariants}
          initial="hidden"
          whileInView="visible"
          viewport={{ once: true }}
        >
          {features.map((feature, index) => (
            <motion.div 
              key={index}
              className="feature-card card"
              variants={itemVariants}
              whileHover={{ 
                scale: 1.05,
                transition: { duration: 0.2 }
              }}
            >
              <div 
                className="feature-icon"
                style={{ color: feature.color }}
              >
                {feature.icon}
              </div>
              <h3 className="feature-title">{feature.title}</h3>
              <p className="feature-description">{feature.description}</p>
            </motion.div>
          ))}
        </motion.div>

        <motion.div 
          className="features-highlight"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.3 }}
          viewport={{ once: true }}
        >
          <div className="highlight-card card">
            <div className="highlight-icon">
              <span>🔄</span>
            </div>
            <div className="highlight-content">
              <h3>Reliability First</h3>
              <p>
                Version 2.1.0 introduces crash prevention mechanisms and optimized communication 
                protocols to ensure your afterburner controller runs smoothly without interruptions.
              </p>
              <div className="highlight-stats">
                <div className="stat">
                  <span className="stat-number">99.9%</span>
                  <span className="stat-label">Uptime</span>
                </div>
                <div className="stat">
                  <span className="stat-number">0</span>
                  <span className="stat-label">Crashes</span>
                </div>
                <div className="stat">
                  <span className="stat-number">300</span>
                  <span className="stat-label">Max LEDs</span>
                </div>
              </div>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Features;
