# Testing Guide for AfterburnerControl

This directory contains comprehensive tests for the AfterburnerControl React Native application.

## 📁 Test Structure

```
src/__tests__/
├── setup.ts                    # Test environment setup and mocks
├── components/                 # Component tests
│   └── ColorInput.test.tsx    # ColorInput component tests
├── screens/                    # Screen tests
│   └── AfterburnerScreen.test.tsx  # Main screen tests
├── ble/                        # BLE functionality tests
│   └── bleManager.test.ts     # BLE manager tests
├── utils/                      # Utility function tests
│   └── colorUtils.test.ts     # Color utility tests
├── integration/                # Integration tests
│   └── bleIntegration.test.ts # End-to-end BLE workflow tests
└── runTests.ts                 # Test runner utility
```

## 🧪 Test Categories

### 1. Component Tests (`components/`)

- **ColorInput.test.tsx**: Tests the color picker component
  - Color display and formatting
  - Modal interactions
  - Color selection and validation
  - Send button functionality
  - Edge cases (black, white, invalid colors)

### 2. Screen Tests (`screens/`)

- **AfterburnerScreen.test.tsx**: Tests the main application screen
  - UI rendering and state management
  - BLE connection workflow
  - User interactions (buttons, sliders)
  - Error handling and status display
  - Component lifecycle management

### 3. BLE Tests (`ble/`)

- **bleManager.test.ts**: Tests Bluetooth Low Energy functionality
  - BLE initialization and state management
  - Device scanning and discovery
  - Connection and disconnection
  - Data transmission (colors, settings)
  - Error handling and recovery

### 4. Utility Tests (`utils/`)

- **colorUtils.test.ts**: Tests color manipulation functions
  - RGB to Hex conversion
  - Hex to RGB conversion
  - Color validation
  - Color interpolation
  - Brightness calculation

### 5. Integration Tests (`integration/`)

- **bleIntegration.test.ts**: Tests complete workflows
  - End-to-end BLE communication
  - Data transmission workflows
  - Error recovery scenarios
  - Performance and timing tests

## 🚀 Running Tests

### Basic Commands

```bash
# Run all tests
npm test

# Run tests in watch mode
npm run test:watch

# Run tests with coverage
npm run test:coverage

# Run tests for CI/CD
npm run test:ci

# Run quality checks (lint + tests)
npm run test:quality
```

### Advanced Test Runner

```bash
# Run all tests
npm run test:runner all

# Run tests with coverage
npm run test:runner coverage

# Run tests in watch mode
npm run test:runner watch

# Generate test report
npm run test:runner report

# Run linting
npm run test:runner lint

# Run type checking
npm run test:runner types

# Run all quality checks
npm run test:runner quality

# Run specific test file
npm run test:runner components/ColorInput.test.tsx
```

## 📊 Coverage Requirements

The test suite is configured with the following coverage thresholds:

- **Branches**: 70%
- **Functions**: 70%
- **Lines**: 70%
- **Statements**: 70%

## 🔧 Test Configuration

### Jest Configuration (`jest.config.js`)

- React Native preset
- Custom setup file for mocks
- Coverage collection from `src/**/*.{ts,tsx}`
- Transform ignore patterns for React Native modules
- Test environment: jsdom

### Test Setup (`setup.ts`)

- Mocks for `react-native-ble-plx`
- Mocks for `@react-native-community/slider`
- Mocks for `react-native-safe-area-context`
- Platform mocking
- Console method mocking
- Development environment setup

## 🎯 Test Coverage Areas

### UI Components

- ✅ Component rendering
- ✅ User interactions
- ✅ State changes
- ✅ Props validation
- ✅ Event handling
- ✅ Modal interactions
- ✅ Color picker functionality

### BLE Functionality

- ✅ BLE initialization
- ✅ Device scanning
- ✅ Connection management
- ✅ Data transmission
- ✅ Error handling
- ✅ State management
- ✅ Disconnection cleanup

### Color Management

- ✅ RGB/Hex conversion
- ✅ Color validation
- ✅ Color interpolation
- ✅ Brightness calculation
- ✅ Edge case handling

### Integration Workflows

- ✅ Complete BLE workflows
- ✅ Error recovery
- ✅ Performance testing
- ✅ Timing validation

## 🐛 Debugging Tests

### Common Issues

1. **Mock Issues**: Ensure all external dependencies are properly mocked
2. **Async Tests**: Use `waitFor` for asynchronous operations
3. **Component Testing**: Add `testID` props for reliable element selection
4. **BLE Mocking**: Verify BLE manager mocks are correctly configured

### Debug Commands

```bash
# Run tests with verbose output
npm test -- --verbose

# Run specific test with debugging
npm test -- --verbose --testNamePattern="ColorInput"

# Run tests with coverage and watch
npm test -- --coverage --watch

# Debug Jest configuration
npm test -- --showConfig
```

## 📝 Writing New Tests

### Component Test Template

```typescript
import React from 'react';
import { render, fireEvent, waitFor } from '@testing-library/react-native';
import { YourComponent } from '../YourComponent';

describe('YourComponent', () => {
  const mockProps = {
    // Define your props here
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  it('renders correctly', () => {
    const { getByText } = render(<YourComponent {...mockProps} />);
    expect(getByText('Expected Text')).toBeTruthy();
  });

  it('handles user interactions', async () => {
    const { getByText } = render(<YourComponent {...mockProps} />);

    fireEvent.press(getByText('Button Text'));

    await waitFor(() => {
      expect(mockCallback).toHaveBeenCalled();
    });
  });
});
```

### BLE Test Template

```typescript
import { YourBLEManager } from '../ble/YourBLEManager';

describe('YourBLEManager', () => {
  let bleManager: YourBLEManager;

  beforeEach(() => {
    jest.clearAllMocks();
    bleManager = new YourBLEManager();
  });

  it('should handle BLE operations', async () => {
    // Setup mocks
    mockBleManager.someMethod.mockResolvedValue(true);

    // Test the operation
    const result = await bleManager.someOperation();

    // Verify results
    expect(result).toBe(true);
    expect(mockBleManager.someMethod).toHaveBeenCalled();
  });
});
```

## 🔄 Continuous Integration

Tests are automatically run in CI/CD pipelines:

- **GitHub Actions**: Runs on every push and pull request
- **Coverage Reports**: Generated and uploaded as artifacts
- **Quality Gates**: Ensures minimum coverage thresholds are met
- **Test Results**: Available in GitHub Actions logs

## 📈 Performance Testing

The test suite includes performance tests for:

- BLE operation timing
- Component rendering performance
- Memory usage validation
- Rapid operation handling

## 🎨 Visual Testing

For visual regression testing, consider adding:

- Screenshot testing with `@testing-library/react-native`
- Visual diff tools
- Component storybook integration

## 📚 Additional Resources

- [React Native Testing Library](https://callstack.github.io/react-native-testing-library/)
- [Jest Documentation](https://jestjs.io/docs/getting-started)
- [React Native Testing Guide](https://reactnative.dev/docs/testing)
- [BLE Testing Best Practices](https://developer.android.com/guide/topics/connectivity/bluetooth/testing)

## 🤝 Contributing

When adding new features:

1. Write tests first (TDD approach)
2. Ensure coverage thresholds are met
3. Add integration tests for new workflows
4. Update this documentation
5. Run the full test suite before submitting

## 📞 Support

For test-related issues:

1. Check the troubleshooting section above
2. Review Jest and React Native Testing Library documentation
3. Check GitHub Actions logs for CI/CD issues
4. Open an issue with detailed error information
