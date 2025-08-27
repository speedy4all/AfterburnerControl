describe('Color Utilities', () => {
  describe('RGB to Hex conversion', () => {
    it('should convert RGB values to hex correctly', () => {
      const rgbToHex = (r: number, g: number, b: number): string => {
        return `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}`;
      };

      expect(rgbToHex(255, 0, 0)).toBe('#ff0000');
      expect(rgbToHex(0, 255, 0)).toBe('#00ff00');
      expect(rgbToHex(0, 0, 255)).toBe('#0000ff');
      expect(rgbToHex(255, 255, 255)).toBe('#ffffff');
      expect(rgbToHex(0, 0, 0)).toBe('#000000');
      expect(rgbToHex(128, 128, 128)).toBe('#808080');
    });

    it('should handle edge cases', () => {
      const rgbToHex = (r: number, g: number, b: number): string => {
        return `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}`;
      };

      expect(rgbToHex(1, 1, 1)).toBe('#010101');
      expect(rgbToHex(15, 15, 15)).toBe('#0f0f0f');
      expect(rgbToHex(16, 16, 16)).toBe('#101010');
    });
  });

  describe('Hex to RGB conversion', () => {
    it('should convert hex values to RGB correctly', () => {
      const hexToRgb = (hex: string): { r: number; g: number; b: number } => {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
          r: parseInt(result[1], 16),
          g: parseInt(result[2], 16),
          b: parseInt(result[3], 16),
        } : { r: 0, g: 0, b: 0 };
      };

      expect(hexToRgb('#ff0000')).toEqual({ r: 255, g: 0, b: 0 });
      expect(hexToRgb('#00ff00')).toEqual({ r: 0, g: 255, b: 0 });
      expect(hexToRgb('#0000ff')).toEqual({ r: 0, g: 0, b: 255 });
      expect(hexToRgb('#ffffff')).toEqual({ r: 255, g: 255, b: 255 });
      expect(hexToRgb('#000000')).toEqual({ r: 0, g: 0, b: 0 });
      expect(hexToRgb('#808080')).toEqual({ r: 128, g: 128, b: 128 });
    });

    it('should handle hex values without #', () => {
      const hexToRgb = (hex: string): { r: number; g: number; b: number } => {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
          r: parseInt(result[1], 16),
          g: parseInt(result[2], 16),
          b: parseInt(result[3], 16),
        } : { r: 0, g: 0, b: 0 };
      };

      expect(hexToRgb('ff0000')).toEqual({ r: 255, g: 0, b: 0 });
      expect(hexToRgb('00ff00')).toEqual({ r: 0, g: 255, b: 0 });
      expect(hexToRgb('0000ff')).toEqual({ r: 0, g: 0, b: 255 });
    });

    it('should handle invalid hex values', () => {
      const hexToRgb = (hex: string): { r: number; g: number; b: number } => {
        const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? {
          r: parseInt(result[1], 16),
          g: parseInt(result[2], 16),
          b: parseInt(result[3], 16),
        } : { r: 0, g: 0, b: 0 };
      };

      expect(hexToRgb('invalid')).toEqual({ r: 0, g: 0, b: 0 });
      expect(hexToRgb('#123')).toEqual({ r: 0, g: 0, b: 0 });
      expect(hexToRgb('')).toEqual({ r: 0, g: 0, b: 0 });
    });
  });

  describe('Color validation', () => {
    it('should validate RGB values correctly', () => {
      const isValidRgb = (r: number, g: number, b: number): boolean => {
        return r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255;
      };

      expect(isValidRgb(0, 0, 0)).toBe(true);
      expect(isValidRgb(255, 255, 255)).toBe(true);
      expect(isValidRgb(128, 128, 128)).toBe(true);
      expect(isValidRgb(-1, 0, 0)).toBe(false);
      expect(isValidRgb(256, 0, 0)).toBe(false);
      expect(isValidRgb(0, -1, 0)).toBe(false);
      expect(isValidRgb(0, 256, 0)).toBe(false);
      expect(isValidRgb(0, 0, -1)).toBe(false);
      expect(isValidRgb(0, 0, 256)).toBe(false);
    });

    it('should validate hex values correctly', () => {
      const isValidHex = (hex: string): boolean => {
        return /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.test(hex);
      };

      expect(isValidHex('#ff0000')).toBe(true);
      expect(isValidHex('#00ff00')).toBe(true);
      expect(isValidHex('#0000ff')).toBe(true);
      expect(isValidHex('#ffffff')).toBe(true);
      expect(isValidHex('#000000')).toBe(true);
      expect(isValidHex('ff0000')).toBe(true);
      expect(isValidHex('invalid')).toBe(false);
      expect(isValidHex('#123')).toBe(false);
      expect(isValidHex('')).toBe(false);
    });
  });

  describe('Color interpolation', () => {
    it('should interpolate between two colors correctly', () => {
      const interpolateColor = (
        color1: { r: number; g: number; b: number },
        color2: { r: number; g: number; b: number },
        factor: number
      ): { r: number; g: number; b: number } => {
        return {
          r: Math.round(color1.r + (color2.r - color1.r) * factor),
          g: Math.round(color1.g + (color2.g - color1.g) * factor),
          b: Math.round(color1.b + (color2.b - color1.b) * factor),
        };
      };

      const red = { r: 255, g: 0, b: 0 };
      const blue = { r: 0, g: 0, b: 255 };

      expect(interpolateColor(red, blue, 0)).toEqual(red);
      expect(interpolateColor(red, blue, 1)).toEqual(blue);
      expect(interpolateColor(red, blue, 0.5)).toEqual({ r: 128, g: 0, b: 128 });
    });

    it('should handle edge cases in interpolation', () => {
      const interpolateColor = (
        color1: { r: number; g: number; b: number },
        color2: { r: number; g: number; b: number },
        factor: number
      ): { r: number; g: number; b: number } => {
        return {
          r: Math.round(color1.r + (color2.r - color1.r) * factor),
          g: Math.round(color1.g + (color2.g - color1.g) * factor),
          b: Math.round(color1.b + (color2.b - color1.b) * factor),
        };
      };

      const black = { r: 0, g: 0, b: 0 };
      const white = { r: 255, g: 255, b: 255 };

      expect(interpolateColor(black, white, 0.25)).toEqual({ r: 64, g: 64, b: 64 });
      expect(interpolateColor(black, white, 0.75)).toEqual({ r: 191, g: 191, b: 191 });
    });
  });

  describe('Color brightness calculation', () => {
    it('should calculate brightness correctly', () => {
      const calculateBrightness = (r: number, g: number, b: number): number => {
        return (r * 299 + g * 587 + b * 114) / 1000;
      };

      expect(calculateBrightness(255, 255, 255)).toBe(255);
      expect(calculateBrightness(0, 0, 0)).toBe(0);
      expect(calculateBrightness(128, 128, 128)).toBe(128);
      expect(calculateBrightness(255, 0, 0)).toBe(76.245);
      expect(calculateBrightness(0, 255, 0)).toBe(149.685);
      expect(calculateBrightness(0, 0, 255)).toBe(29.07);
    });

    it('should determine if color is light or dark', () => {
      const isLightColor = (r: number, g: number, b: number): boolean => {
        const brightness = (r * 299 + g * 587 + b * 114) / 1000;
        return brightness > 128;
      };

      expect(isLightColor(255, 255, 255)).toBe(true);
      expect(isLightColor(0, 0, 0)).toBe(false);
      expect(isLightColor(128, 128, 128)).toBe(false);
      expect(isLightColor(200, 200, 200)).toBe(true);
      expect(isLightColor(100, 100, 100)).toBe(false);
    });
  });
});
