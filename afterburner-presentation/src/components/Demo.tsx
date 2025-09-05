import React, { useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { FaPlay, FaPause, FaExpand, FaDownload, FaGithub } from 'react-icons/fa';
import { IconType } from 'react-icons';
import './Demo.css';

const Demo: React.FC = () => {
  const [activeDemo, setActiveDemo] = useState(0);
  const [isPlaying, setIsPlaying] = useState(false);

  // Icon wrapper to fix React 19 compatibility
  const PlayIcon = () => <span>▶</span>;
  const PauseIcon = () => <span>⏸</span>;

  const demos = [
    {
      id: 1,
      title: "Static Mode Demo",
      description: "Solid color with throttle-responsive brightness",
      type: "video",
      thumbnail: "/api/placeholder/400/300",
      videoUrl: "#",
      duration: "0:30"
    },
    {
      id: 2,
      title: "Pulse Mode Demo", 
      description: "Smooth pulsing effect with throttle-responsive intensity",
      type: "video",
      thumbnail: "/api/placeholder/400/300",
      videoUrl: "#",
      duration: "0:45"
    },
    {
      id: 3,
      title: "Afterburner Mode Demo",
      description: "Dynamic afterburner effect with sparkles and color transitions",
      type: "video", 
      thumbnail: "/api/placeholder/400/300",
      videoUrl: "#",
      duration: "1:00"
    },
    {
      id: 4,
      title: "Mobile App Interface",
      description: "Real-time control via React Native app",
      type: "image",
      thumbnail: "/api/placeholder/400/300",
      fullImage: "/api/placeholder/800/600"
    },
    {
      id: 5,
      title: "Hardware Setup",
      description: "ESP8266 NodeMCU V3 with WS2812B LED strip",
      type: "image",
      thumbnail: "/api/placeholder/400/300", 
      fullImage: "/api/placeholder/800/600"
    },
    {
      id: 6,
      title: "Calibration Process",
      description: "Automatic throttle calibration in action",
      type: "video",
      thumbnail: "/api/placeholder/400/300",
      videoUrl: "#",
      duration: "0:20"
    }
  ];

  const handlePlayPause = () => {
    setIsPlaying(!isPlaying);
  };

  const handleDemoSelect = (index: number) => {
    setActiveDemo(index);
    setIsPlaying(false);
  };

  return (
    <section id="demo" className="demo section">
      <div className="container">
        <motion.div 
          className="demo-header"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8 }}
          viewport={{ once: true }}
        >
          <h2 className="section-title">
            <span className="gradient-text">See It In Action</span>
          </h2>
          <p className="section-description">
            Watch the ESP8266 Afterburner Controller in action with real demos and examples
          </p>
        </motion.div>

        <div className="demo-content">
          <motion.div 
            className="demo-main"
            initial={{ opacity: 0, x: -30 }}
            whileInView={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.8 }}
            viewport={{ once: true }}
          >
            <div className="demo-player card">
              <div className="player-header">
                <h3>{demos[activeDemo].title}</h3>
                <div className="player-controls">
                  {demos[activeDemo].type === 'video' && (
                    <>
                      <button 
                        className="control-btn"
                        onClick={handlePlayPause}
                      >
                        {isPlaying ? <PauseIcon /> : <PlayIcon />}
                      </button>
                      <span className="duration">{demos[activeDemo].duration}</span>
                    </>
                  )}
                  <button className="control-btn">
                    <span>⛶</span>
                  </button>
                </div>
              </div>
              
              <div className="player-content">
                <div className="media-container">
                  <img 
                    src={demos[activeDemo].thumbnail}
                    alt={demos[activeDemo].title}
                    className="demo-media"
                  />
                  {demos[activeDemo].type === 'video' && (
                    <div className="play-overlay">
                      <button 
                        className="play-button"
                        onClick={handlePlayPause}
                      >
                        {isPlaying ? <PauseIcon /> : <PlayIcon />}
                      </button>
                    </div>
                  )}
                </div>
                
                <div className="player-info">
                  <p>{demos[activeDemo].description}</p>
                  <div className="player-actions">
                    <a href="#" className="btn btn-secondary">
                      <span>⬇</span>
                      Download
                    </a>
                    <a href="#" className="btn btn-secondary">
                      <span>📁</span>
                      Source
                    </a>
                  </div>
                </div>
              </div>
            </div>
          </motion.div>

          <motion.div 
            className="demo-sidebar"
            initial={{ opacity: 0, x: 30 }}
            whileInView={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.8, delay: 0.2 }}
            viewport={{ once: true }}
          >
            <h4>More Demos</h4>
            <div className="demo-thumbnails">
              {demos.map((demo, index) => (
                <motion.div
                  key={demo.id}
                  className={`demo-thumbnail ${activeDemo === index ? 'active' : ''}`}
                  onClick={() => handleDemoSelect(index)}
                  whileHover={{ scale: 1.05 }}
                  whileTap={{ scale: 0.95 }}
                >
                  <div className="thumbnail-image">
                    <img src={demo.thumbnail} alt={demo.title} />
                    {demo.type === 'video' && (
                      <div className="video-indicator">
                        <span>▶</span>
                      </div>
                    )}
                  </div>
                  <div className="thumbnail-info">
                    <h5>{demo.title}</h5>
                    <p>{demo.description}</p>
                    {demo.type === 'video' && (
                      <span className="duration">{demo.duration}</span>
                    )}
                  </div>
                </motion.div>
              ))}
            </div>
          </motion.div>
        </div>

        <motion.div 
          className="demo-stats"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 0.8, delay: 0.4 }}
          viewport={{ once: true }}
        >
          <div className="stats-grid">
            <div className="stat-item">
              <div className="stat-icon">🎬</div>
              <div className="stat-content">
                <h4>6 Demo Videos</h4>
                <p>Comprehensive coverage of all features</p>
              </div>
            </div>
            <div className="stat-item">
              <div className="stat-icon">📱</div>
              <div className="stat-content">
                <h4>Mobile App</h4>
                <p>Real-time control interface</p>
              </div>
            </div>
            <div className="stat-item">
              <div className="stat-icon">⚡</div>
              <div className="stat-content">
                <h4>Real-time</h4>
                <p>Instant response to throttle input</p>
              </div>
            </div>
            <div className="stat-item">
              <div className="stat-icon">🔧</div>
              <div className="stat-content">
                <h4>Open Source</h4>
                <p>Full source code available</p>
              </div>
            </div>
          </div>
        </motion.div>
      </div>
    </section>
  );
};

export default Demo;
