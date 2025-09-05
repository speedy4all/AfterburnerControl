import React from 'react';
import { motion } from 'framer-motion';
import './App.css';
import Hero from './components/Hero';
import Features from './components/Features';
import Demo from './components/Demo';
import Downloads from './components/Downloads';
import Footer from './components/Footer';

function App() {
  return (
    <div className="App">
      <Hero />
      <Features />
      <Demo />
      <Downloads />
      <Footer />
    </div>
  );
}

export default App;