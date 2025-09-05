# GitHub Actions Workflows

This directory contains the GitHub Actions workflows for the Afterburner project.

## Workflow Overview

### 1. **test.yml** - React Native Tests

- **Purpose**: Runs React Native application tests
- **Trigger**: All pushes and pull requests
- **What it does**:
  - Sets up Node.js environment
  - Installs dependencies
  - Runs React Native tests with coverage
  - Uploads test coverage artifacts

### 2. **firmware-test.yml** - Firmware Testing

- **Purpose**: Tests ESP32 firmware builds and validation
- **Trigger**: Only when firmware source files change
- **What it does**:
  - Sets up PlatformIO environment
  - Builds normal firmware (`esp32dev`)
  - Builds test firmware (`esp32dev_test`)
  - Verifies both builds succeed
  - Analyzes firmware sizes
  - Uploads firmware artifacts
  - Generates test reports

### 3. **react-native-build.yml** - React Native Builds

- **Purpose**: Builds React Native app for Android and iOS
- **Trigger**: Only when React Native source files change
- **What it does**:
  - Builds Android APK
  - Builds iOS app (macOS only)
  - Uploads build artifacts

### 4. **react-native-nightly.yml** - Nightly React Native Builds

- **Purpose**: Scheduled nightly builds of React Native app
- **Trigger**: Every night at 2 AM UTC + manual dispatch
- **What it does**:
  - Builds Android APK and iOS app
  - Creates detailed build reports
  - Uploads artifacts with longer retention (30 days)
  - Provides success/failure notifications

### 5. **firmware-nightly.yml** - Nightly Firmware Builds

- **Purpose**: Scheduled nightly builds of ESP32 firmware
- **Trigger**: Every night at 3 AM UTC + manual dispatch
- **What it does**:
  - Builds normal and test firmware
  - Analyzes firmware sizes
  - Creates detailed build reports
  - Uploads artifacts with longer retention (30 days)
  - Provides success/failure notifications

## Workflow Optimization

### ✅ **Removed Redundancies**

- Eliminated duplicate firmware testing in `test.yml`
- Removed outdated `firmware-build.yml` workflow
- Streamlined workflow responsibilities

### ✅ **Improved Efficiency**

- **Path-based triggers**: Workflows only run when relevant files change
- **Caching**: PlatformIO dependencies are cached for faster builds
- **Focused testing**: Each workflow has a specific purpose
- **Scheduled builds**: Nightly builds reduce manual triggering

### ✅ **Better Organization**

- **Firmware testing**: Handled by `firmware-test.yml`
- **React Native testing**: Handled by `test.yml`
- **React Native building**: Handled by `react-native-build.yml`
- **Nightly builds**: Separate scheduled workflows for regular builds

## Workflow Triggers

| Workflow                   | Trigger        | Path Filter                 | Schedule       |
| -------------------------- | -------------- | --------------------------- | -------------- |
| `test.yml`                 | All pushes/PRs | None (runs always)          | -              |
| `firmware-test.yml`        | All pushes/PRs | `firmware/src/**`           | -              |
| `react-native-build.yml`   | All pushes/PRs | `AfterburnerControl/src/**` | -              |
| `react-native-nightly.yml` | Manual         | None                        | 2 AM UTC daily |
| `firmware-nightly.yml`     | Manual         | None                        | 3 AM UTC daily |

## Benefits

### 🚀 **Performance**

- Faster CI/CD pipeline execution
- Reduced resource usage
- Parallel workflow execution

### 🎯 **Focused Testing**

- Firmware changes only trigger firmware tests
- React Native changes only trigger app tests
- No unnecessary workflow runs

### 📊 **Better Reporting**

- Clear separation of concerns
- Specific artifact uploads
- Detailed test reports

## Usage

### For Firmware Development

- Push firmware changes → `firmware-test.yml` runs automatically
- Check build status and download firmware artifacts
- Review test reports for hardware testing guidance

### For React Native Development

- Push app changes → `test.yml` and `react-native-build.yml` run automatically
- Check test coverage and build artifacts
- Download APK/iOS builds for testing

### For Full Project Changes

- All workflows run to ensure complete project integrity
- Comprehensive testing across all components

## Artifacts

### Firmware Testing

- `firmware-builds`: Compiled firmware files (.bin, .elf)
- `test-report`: Detailed test coverage report

### React Native Testing

- `test-coverage`: Test coverage reports

### React Native Building

- `android-apk`: Android APK files
- `ios-build`: iOS build artifacts

### Nightly Builds

- `android-apk-nightly`: Nightly Android APK builds (30-day retention)
- `ios-build-nightly`: Nightly iOS build artifacts (30-day retention)
- `firmware-nightly-builds`: Nightly firmware builds (30-day retention)
- `nightly-build-report`: React Native nightly build reports
- `firmware-nightly-report`: Firmware nightly build reports

## Troubleshooting

### Workflow Not Running

1. Check if file changes match the path filters
2. Verify the workflow is in the correct branch
3. Check GitHub Actions permissions

### Build Failures

1. Review workflow logs for specific errors
2. Check dependency versions and compatibility
3. Verify platform-specific requirements

### Performance Issues

1. Check cache hit rates
2. Review workflow optimization opportunities
3. Consider parallel job execution
