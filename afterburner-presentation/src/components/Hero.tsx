import React from 'react';
import { motion } from 'framer-motion';
import { FaRocket, FaWifi, FaMobile, FaCog } from 'react-icons/fa';
import './Hero.css';

const Hero: React.FC = () => {
  return (
    <section className="hero">
      <div className="hero-background">
        <div className="stars"></div>
        <div className="nebula"></div>
      </div>
      
      <div className="container">
        <motion.div 
          className="hero-content"
          initial={{ opacity: 0, y: 50 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
        >
          <motion.div 
            className="hero-badge"
            initial={{ opacity: 0, scale: 0.8 }}
            animate={{ opacity: 1, scale: 1 }}
            transition={{ delay: 0.2, duration: 0.5 }}
          >
            <span className="badge-icon">🚀</span>
            <span>ESP8266 NodeMCU V3 Edition</span>
          </motion.div>

          <motion.h1 
            className="hero-title"
            initial={{ opacity: 0, y: 30 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.3, duration: 0.8 }}
          >
            <span className="gradient-text">ESP8266 Afterburner</span>
            <br />
            Controller
          </motion.h1>

          <motion.p 
            className="hero-description"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.5, duration: 0.8 }}
          >
            A sophisticated LED afterburner controller for RC models featuring automatic throttle calibration, 
            real-time LED effects, and wireless control via React Native app.
          </motion.p>

          <motion.div 
            className="hero-features"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.7, duration: 0.8 }}
          >
            <div className="feature-item">
              <span className="feature-icon">📶</span>
              <span>Wireless Control</span>
            </div>
            <div className="feature-item">
              <span className="feature-icon">📱</span>
              <span>Mobile App</span>
            </div>
            <div className="feature-item">
              <span className="feature-icon">⚙️</span>
              <span>Auto Calibration</span>
            </div>
          </motion.div>

          <motion.div 
            className="hero-actions"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            transition={{ delay: 0.9, duration: 0.8 }}
          >
            <a href="#downloads" className="btn btn-primary">
              Download App
            </a>
            <a href="#features" className="btn btn-secondary">
              Learn More
            </a>
          </motion.div>
        </motion.div>

        <motion.div 
          className="hero-visual"
          initial={{ opacity: 0, x: 50 }}
          animate={{ opacity: 1, x: 0 }}
          transition={{ delay: 0.4, duration: 1 }}
        >
          <div className="device-mockup">
            <div className="device-screen">
              <div className="app-interface">
                <div className="app-header">
                  <div className="status-indicator"></div>
                  <span>Afterburner Control</span>
                </div>
                <div className="app-content">
                  <div className="control-panel">
                    <div className="mode-selector">
                      <div className="mode active">Static</div>
                      <div className="mode">Pulse</div>
                      <div className="mode">Afterburner</div>
                    </div>
                    <div className="color-picker">
                      <div className="color-circle" style={{ backgroundColor: '#ff6b6b' }}></div>
                      <div className="color-circle" style={{ backgroundColor: '#4ecdc4' }}></div>
                    </div>
                    <div className="slider-control">
                      <div className="slider-track">
                        <div className="slider-thumb" style={{ left: '60%' }}></div>
                      </div>
                      <span>Brightness</span>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Hero;
