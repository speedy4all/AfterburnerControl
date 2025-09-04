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
  color: { r: number; g: number; b: number };
  onColorChange: (color: { r: number; g: number; b: number }) => void;
  onSend?: () => void;
  isDarkTheme?: boolean;
}

export const ColorInput: React.FC<ColorInputProps> = ({
  label,
  color,
  onColorChange,
  onSend,
  isDarkTheme = false,
}) => {
  const [showColorPicker, setShowColorPicker] = useState(false);

  const handleColorChange = (newColor: { r: number; g: number; b: number }) => {
    onColorChange(newColor);
  };

  const handleSend = () => {
    if (onSend) {
      onSend();
    }
  };

  const colorStyle = {
    backgroundColor: `rgb(${color.r}, ${color.g}, ${color.b})`,
  };

  const colorHex = `#${color.r.toString(16).padStart(2, '0')}${color.g.toString(16).padStart(2, '0')}${color.b.toString(16).padStart(2, '0')}`;

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
    <View style={[styles.container, isDarkTheme && styles.containerDark]}>
      <View style={styles.header}>
        <Text style={[styles.label, isDarkTheme && styles.labelDark]}>{label}</Text>
        <TouchableOpacity
          style={[styles.colorSwatch, isDarkTheme && styles.colorSwatchDark, colorStyle]}
          onPress={() => setShowColorPicker(true)}
        />
      </View>
      
      <View style={styles.infoRow}>
        <Text style={[styles.colorInfo, isDarkTheme && styles.colorInfoDark]}>
          RGB({color.r}, {color.g}, {color.b})
        </Text>
        <Text style={[styles.colorInfo, isDarkTheme && styles.colorInfoDark]}>{colorHex}</Text>
      </View>

      {onSend && (
        <TouchableOpacity style={styles.sendButton} onPress={handleSend}>
          <Text style={styles.sendButtonText}>Send</Text>
        </TouchableOpacity>
      )}

      <Modal
        visible={showColorPicker}
        transparent={true}
        animationType="slide"
        onRequestClose={() => setShowColorPicker(false)}
      >
        <View style={styles.modalOverlay}>
          <View style={[styles.modalContent, isDarkTheme && styles.modalContentDark]}>
            <View style={[styles.modalHeader, isDarkTheme && styles.modalHeaderDark]}>
              <Text style={[styles.modalTitle, isDarkTheme && styles.modalTitleDark]}>Select {label}</Text>
              <TouchableOpacity
                style={styles.closeButton}
                onPress={() => setShowColorPicker(false)}
              >
                <Text style={styles.closeButtonText}>Done</Text>
              </TouchableOpacity>
            </View>
            
            <ScrollView style={styles.colorPickerContainer}>
              {/* Predefined Colors */}
              <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Quick Colors</Text>
              <View style={styles.predefinedColors}>
                {predefinedColors.map((predefinedColor, index) => (
                  <TouchableOpacity
                    key={index}
                    style={[
                      styles.predefinedColor,
                      isDarkTheme && styles.predefinedColorDark,
                      { backgroundColor: `rgb(${predefinedColor.r}, ${predefinedColor.g}, ${predefinedColor.b})` }
                    ]}
                    onPress={() => handleColorChange(predefinedColor)}
                  />
                ))}
              </View>

              {/* RGB Sliders */}
              <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Custom RGB</Text>
              
              <View style={styles.sliderContainer}>
                <Text style={[styles.sliderLabel, isDarkTheme && styles.sliderLabelDark]}>Red: {color.r} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={color.r}
                  onValueChange={(value) => handleColorChange({ ...color, r: Math.round(value) })}
                  minimumTrackTintColor="#FF0000"
                  maximumTrackTintColor="#555555"
                  thumbStyle={{ backgroundColor: '#FF0000' }}
                />
              </View>

              <View style={styles.sliderContainer}>
                <Text style={[styles.sliderLabel, isDarkTheme && styles.sliderLabelDark]}>Green: {color.g} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={color.g}
                  onValueChange={(value) => handleColorChange({ ...color, g: Math.round(value) })}
                  minimumTrackTintColor="#00FF00"
                  maximumTrackTintColor="#555555"
                  thumbStyle={{ backgroundColor: '#00FF00' }}
                />
              </View>

              <View style={styles.sliderContainer}>
                <Text style={[styles.sliderLabel, isDarkTheme && styles.sliderLabelDark]}>Blue: {color.b} (0-255)</Text>
                <Slider
                  style={styles.slider}
                  minimumValue={0}
                  maximumValue={255}
                  value={color.b}
                  onValueChange={(value) => handleColorChange({ ...color, b: Math.round(value) })}
                  minimumTrackTintColor="#0000FF"
                  maximumTrackTintColor="#555555"
                  thumbStyle={{ backgroundColor: '#0000FF' }}
                />
              </View>

              {/* Current Color Preview */}
              <View style={[styles.currentColorContainer, isDarkTheme && styles.currentColorContainerDark]}>
                <Text style={[styles.sectionTitle, isDarkTheme && styles.sectionTitleDark]}>Current Color</Text>
                <View style={[styles.currentColorPreview, isDarkTheme && styles.currentColorPreviewDark, colorStyle]} />
                <Text style={[styles.colorInfo, isDarkTheme && styles.colorInfoDark]}>
                  RGB({color.r}, {color.g}, {color.b})
                </Text>
                <Text style={[styles.colorInfo, isDarkTheme && styles.colorInfoDark]}>{colorHex}</Text>
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
    backgroundColor: '#ffffff', // Light container
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#e0e0e0', // Light border
  },
  containerDark: {
    backgroundColor: '#2d2d2d', // Dark container
    borderColor: '#404040', // Dark border
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
    color: '#333333', // Dark text for light theme
  },
  labelDark: {
    color: '#ffffff', // White text for dark theme
  },
  colorSwatch: {
    width: 50,
    height: 50,
    borderRadius: 25,
    borderWidth: 3,
    borderColor: '#cccccc', // Light border
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 3.84,
    elevation: 5,
  },
  colorSwatchDark: {
    borderColor: '#555555', // Dark border
  },
  infoRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    marginBottom: 8,
  },
  colorInfo: {
    fontSize: 12,
    color: '#666666', // Dark gray text for light theme
    fontFamily: 'monospace',
    textAlign: 'center',
  },
  colorInfoDark: {
    color: '#cccccc', // Light gray text for dark theme
  },
  sendButton: {
    backgroundColor: '#3b82f6', // Blue button
    paddingHorizontal: 16,
    paddingVertical: 8,
    borderRadius: 6,
    alignSelf: 'flex-end',
  },
  sendButtonText: {
    color: 'white',
    fontSize: 14,
    fontWeight: '600',
  },
  modalOverlay: {
    flex: 1,
    backgroundColor: 'rgba(0, 0, 0, 0.7)', // Darker overlay
    justifyContent: 'center',
    alignItems: 'center',
  },
  modalContent: {
    backgroundColor: '#ffffff', // Light modal background
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
  modalContentDark: {
    backgroundColor: '#1a1a1a', // Dark modal background
  },
  modalHeader: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    padding: 16,
    borderBottomWidth: 1,
    borderBottomColor: '#e0e0e0', // Light border
  },
  modalHeaderDark: {
    borderBottomColor: '#404040', // Dark border
  },
  modalTitle: {
    fontSize: 18,
    fontWeight: '600',
    color: '#333333', // Dark text for light theme
  },
  modalTitleDark: {
    color: '#ffffff', // White text for dark theme
  },
  closeButton: {
    backgroundColor: '#3b82f6', // Blue button
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
    color: '#333333', // Dark text for light theme
    marginBottom: 12,
    marginTop: 8,
  },
  sectionTitleDark: {
    color: '#ffffff', // White text for dark theme
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
    borderColor: '#cccccc', // Light border
  },
  predefinedColorDark: {
    borderColor: '#555555', // Dark border
  },
  sliderContainer: {
    marginBottom: 16,
  },
  sliderLabel: {
    fontSize: 14,
    color: '#666666', // Dark gray text for light theme
    marginBottom: 8,
  },
  sliderLabelDark: {
    color: '#cccccc', // Light gray text for dark theme
  },
  slider: {
    width: '100%',
    height: 40,
  },
  currentColorContainer: {
    alignItems: 'center',
    marginTop: 20,
    padding: 16,
    backgroundColor: '#f8f9fa', // Light container
    borderRadius: 8,
    borderWidth: 1,
    borderColor: '#e0e0e0', // Light border
  },
  currentColorContainerDark: {
    backgroundColor: '#2d2d2d', // Dark container
    borderColor: '#404040', // Dark border
  },
  currentColorPreview: {
    width: 80,
    height: 80,
    borderRadius: 40,
    borderWidth: 3,
    borderColor: '#cccccc', // Light border
    marginBottom: 12,
  },
  currentColorPreviewDark: {
    borderColor: '#555555', // Dark border
  },
});
