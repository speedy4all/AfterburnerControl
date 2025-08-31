import React, { useState } from 'react';
import {
  View,
  Text,
  StyleSheet,
  TouchableOpacity,
  Modal,
  Dimensions,
  ScrollView,
} from 'react-native';
import Slider from '@react-native-community/slider';

interface ColorInputProps {
  label: string;
  color: { r: number; g: number; b: number } | [number, number, number];
  onColorChange: (color: { r: number; g: number; b: number }) => void;
}

export const ColorInput: React.FC<ColorInputProps> = ({
  label,
  color,
  onColorChange,
}) => {
  const [showColorPicker, setShowColorPicker] = useState(false);

  // Convert color to standard format
  const normalizedColor = Array.isArray(color) 
    ? { r: color[0] || 0, g: color[1] || 0, b: color[2] || 0 }
    : { r: color.r || 0, g: color.g || 0, b: color.b || 0 };

  const handleColorChange = (newColor: { r: number; g: number; b: number }) => {
    onColorChange(newColor);
  };



  const colorStyle = {
    backgroundColor: `rgb(${normalizedColor.r}, ${normalizedColor.g}, ${normalizedColor.b})`,
  };

  const colorHex = `#${normalizedColor.r.toString(16).padStart(2, '0')}${normalizedColor.g.toString(16).padStart(2, '0')}${normalizedColor.b.toString(16).padStart(2, '0')}`;

  // Predefined color palette
  const predefinedColors = [
    { r: 255, g: 0, b: 0 },     // Red
    { r: 255, g: 165, b: 0 },   // Orange
    { r: 255, g: 255, b: 0 },   // Yellow
    { r: 0, g: 255, b: 0 },     // Green
    { r: 0, g: 255, b: 255 },   // Cyan
    { r: 0, g: 0, b: 255 },     // Blue
    { r: 128, g: 0, b: 128 },   // Purple
    { r: 255, g: 192, b: 203 }, // Pink
    { r: 255, g: 255, b: 255 }, // White
    { r: 128, g: 128, b: 128 }, // Gray
    { r: 0, g: 0, b: 0 },       // Black
    { r: 255, g: 100, b: 0 },   // Ember (from spec)
    { r: 154, g: 0, b: 255 },   // Purple (from spec)
  ];

  return (
    <View style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.label}>{label}</Text>
        <TouchableOpacity
          style={[styles.colorSwatch, colorStyle]}
          onPress={() => setShowColorPicker(true)}
        />
      </View>
      
      <View style={styles.infoRow}>
        <Text style={styles.colorInfo}>
          RGB({normalizedColor.r}, {normalizedColor.g}, {normalizedColor.b})
        </Text>
        <Text style={styles.colorInfo}>{colorHex}</Text>
      </View>



      <Modal
        visible={showColorPicker}
        transparent={true}
        animationType="slide"
        onRequestClose={() => setShowColorPicker(false)}
      >
        <View style={styles.modalOverlay}>
          <View style={styles.modalContent}>
            <View style={styles.modalHeader}>
              <Text style={styles.modalTitle}>Select {label}</Text>
              <TouchableOpacity
                style={styles.closeButton}
                onPress={() => setShowColorPicker(false)}
              >
                <Text style={styles.closeButtonText}>Done</Text>
              </TouchableOpacity>
            </View>
            
            <ScrollView style={styles.colorPickerContainer}>
              {/* Predefined Colors */}
              <Text style={styles.sectionTitle}>Quick Colors</Text>
              <View style={styles.predefinedColors}>
                {predefinedColors.map((predefinedColor, index) => (
                  <TouchableOpacity
                    key={index}
                    style={[
                      styles.predefinedColor,
                      { backgroundColor: `rgb(${predefinedColor.r}, ${predefinedColor.g}, ${predefinedColor.b})` }
                    ]}
                    onPress={() => handleColorChange(predefinedColor)}
                  />
                ))}
              </View>

              {/* RGB Sliders */}
              <Text style={styles.sectionTitle}>Custom RGB</Text>
              
              <View style={styles.sliderContainer}>
                <Text style={styles.sliderLabel}>Red: {normalizedColor.r} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={normalizedColor.r}
                  onValueChange={(value) => handleColorChange({ ...normalizedColor, r: Math.round(value) })}
                  minimumTrackTintColor="#FF0000"
                  maximumTrackTintColor="#CCCCCC"
                />
              </View>

              <View style={styles.sliderContainer}>
                <Text style={styles.sliderLabel}>Green: {normalizedColor.g} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={normalizedColor.g}
                  onValueChange={(value) => handleColorChange({ ...normalizedColor, g: Math.round(value) })}
                  minimumTrackTintColor="#00FF00"
                  maximumTrackTintColor="#CCCCCC"
                />
              </View>

              <View style={styles.sliderContainer}>
                <Text style={styles.sliderLabel}>Blue: {normalizedColor.b} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={normalizedColor.b}
                  onValueChange={(value) => handleColorChange({ ...normalizedColor, b: Math.round(value) })}
                  minimumTrackTintColor="#0000FF"
                  maximumTrackTintColor="#CCCCCC"
                />
              </View>

              {/* Current Color Preview */}
              <View style={styles.currentColorContainer}>
                <Text style={styles.sectionTitle}>Current Color</Text>
                <View style={[styles.currentColorPreview, colorStyle]} />
                <Text style={styles.colorInfo}>
                  RGB({normalizedColor.r}, {normalizedColor.g}, {normalizedColor.b})
                </Text>
                <Text style={styles.colorInfo}>{colorHex}</Text>
              </View>
            </ScrollView>
          </View>
        </View>
      </Modal>
    </View>
  );
};

const { width } = Dimensions.get('window');

const styles = StyleSheet.create({
  container: {
    marginVertical: 8,
    padding: 12,
    backgroundColor: '#f5f5f5',
    borderRadius: 8,
  },
  header: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginBottom: 8,
  },
  label: {
    fontSize: 16,
    fontWeight: '600',
    color: '#333',
  },
  colorSwatch: {
    width: 50,
    height: 50,
    borderRadius: 25,
    borderWidth: 3,
    borderColor: '#ddd',
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    elevation: 5,
  },
  infoRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    marginBottom: 8,
  },
  colorInfo: {
    fontSize: 12,
    color: '#666',
    fontFamily: 'monospace',
    textAlign: 'center',
  },

  modalOverlay: {
    flex: 1,
    backgroundColor: 'rgba(0, 0, 0, 0.5)',
    justifyContent: 'center',
    alignItems: 'center',
  },
  modalContent: {
    backgroundColor: 'white',
    borderRadius: 12,
    width: width * 0.9,
    maxWidth: 400,
    maxHeight: '80%',
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 10,
    },
    shadowOpacity: 0.25,
    shadowRadius: 10,
    elevation: 10,
  },
  modalHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: 16,
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0',
  },
  modalTitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#333',
  },
  closeButton: {
    backgroundColor: '#007AFF',
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 6,
  },
  closeButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  colorPickerContainer: {
    padding: 16,
  },
  sectionTitle: {
    fontSize: 16,
    fontWeight: '600',
    color: '#333',
    marginBottom: 12,
    marginTop: 8,
  },
  predefinedColors: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    justifyContent: 'space-between',
    marginBottom: 20,
  },
  predefinedColor: {
    width: 40,
    height: 40,
    borderRadius: 20,
    margin: 4,
    borderWidth: 2,
    borderColor: '#ddd',
  },
  sliderContainer: {
    marginBottom: 16,
  },
  sliderLabel: {
    fontSize: 14,
    color: '#333',
    marginBottom: 8,
  },
  slider: {
    width: '100%',
    height: 40,
  },
  currentColorContainer: {
    alignItems: 'center',
    marginTop: 20,
    padding: 16,
    backgroundColor: '#f8f8f8',
    borderRadius: 8,
  },
  currentColorPreview: {
    width: 80,
    height: 80,
    borderRadius: 40,
    borderWidth: 3,
    borderColor: '#ddd',
    marginBottom: 12,
  },
});
