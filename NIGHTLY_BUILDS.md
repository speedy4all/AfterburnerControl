# Nightly Builds System

This document explains the nightly build system that automatically runs builds overnight, reducing the need for manual workflow triggering.

## 🕐 Schedule

### React Native Nightly Builds

- **Time**: 2:00 AM UTC daily
- **Workflow**: `react-native-nightly.yml`
- **Purpose**: Build Android APK and iOS app

### Firmware Nightly Builds

- **Time**: 3:00 AM UTC daily (1 hour after React Native)
- **Workflow**: `firmware-nightly.yml`
- **Purpose**: Build ESP32 firmware (normal and test)

## 🎯 Benefits

### ✅ **Reduced Manual Triggering**

- No need to trigger builds on every push
- Automatic builds run overnight
- Manual triggers still available via `workflow_dispatch`

### ✅ **Consistent Build Schedule**

- Predictable build times
- Regular artifact generation
- Consistent testing environment

### ✅ **Longer Artifact Retention**

- Nightly builds kept for 30 days (vs 7 days for push builds)
- Better for testing and debugging
- Historical build artifacts available

### ✅ **Detailed Reports**

- Comprehensive build reports generated
- Size analysis and verification
- Clear next steps for testing

## 📋 What Gets Built

### React Native Nightly Builds

- **Android APK**: Release build for testing
- **iOS App**: Release build for simulator
- **Build Report**: Detailed status and instructions
- **Artifacts**: `android-apk-nightly`, `ios-build-nightly`, `nightly-build-report`

### Firmware Nightly Builds

- **Normal Firmware**: Production ESP32 firmware
- **Test Firmware**: ESP32 firmware with test mode enabled
- **Size Analysis**: Firmware size verification
- **Build Report**: Detailed status and testing instructions
- **Artifacts**: `firmware-nightly-builds`, `firmware-nightly-report`

## 🔧 Manual Triggering

### Via GitHub UI

1. Go to Actions tab
2. Select the nightly workflow
3. Click "Run workflow"
4. Choose branch and click "Run workflow"

### Via GitHub CLI

```bash
# Trigger React Native nightly build
gh workflow run react-native-nightly.yml

# Trigger Firmware nightly build
gh workflow run firmware-nightly.yml
```

## 📊 Workflow Comparison

| Aspect            | Push-Triggered | Nightly               |
| ----------------- | -------------- | --------------------- |
| **Trigger**       | File changes   | Schedule (2-3 AM UTC) |
| **Frequency**     | On every push  | Daily                 |
| **Retention**     | 7 days         | 30 days               |
| **Reports**       | Basic          | Detailed              |
| **Notifications** | Standard       | Success/Failure       |

## 🚀 Usage Workflow

### For Daily Development

1. **Push changes** → Only tests run (fast feedback)
2. **Nightly builds** → Full builds run automatically
3. **Morning check** → Download nightly artifacts for testing

### For Testing

1. **Download nightly APK** → Test on Android device
2. **Download nightly firmware** → Upload to ESP32
3. **Review build reports** → Check for issues

### For Releases

1. **Use nightly builds** → As pre-release testing
2. **Manual trigger** → For final release builds
3. **Long retention** → Keep release candidates

## ⚙️ Configuration

### Schedule Adjustment

To change the schedule, edit the cron expressions:

```yaml
# React Native (currently 2 AM UTC)
- cron: "0 2 * * *"

# Firmware (currently 3 AM UTC)
- cron: "0 3 * * *"
```

### Timezone Conversion

- **2 AM UTC** = 10 PM EST / 7 PM PST
- **3 AM UTC** = 11 PM EST / 8 PM PST

### Retention Periods

- **Nightly artifacts**: 30 days
- **Nightly reports**: 7 days
- **Push artifacts**: 7 days

## 🔍 Monitoring

### Success Notifications

- Workflow completion messages
- Artifact availability confirmation
- Build status summary

### Failure Notifications

- Error messages in workflow logs
- Failure notifications
- Debugging guidance

### Artifact Access

- Download from GitHub Actions artifacts
- Available for 30 days
- Organized by build date

## 📈 Best Practices

### Development Workflow

1. **Push frequently** → Get fast test feedback
2. **Check nightly builds** → Verify full builds work
3. **Use nightly artifacts** → For integration testing

### Testing Strategy

1. **Unit tests** → Run on every push
2. **Integration tests** → Use nightly builds
3. **Manual testing** → Use nightly artifacts

### Release Process

1. **Nightly builds** → Pre-release validation
2. **Manual builds** → Final release artifacts
3. **Artifact retention** → Keep release candidates

## 🛠️ Troubleshooting

### Build Failures

1. Check workflow logs for errors
2. Verify dependency versions
3. Test locally if needed
4. Check for platform-specific issues

### Schedule Issues

1. Verify cron syntax
2. Check GitHub Actions permissions
3. Ensure repository is active
4. Monitor workflow history

### Artifact Issues

1. Check retention periods
2. Verify artifact names
3. Download before expiration
4. Use manual triggers if needed

## 📞 Support

For issues with nightly builds:

1. Check workflow logs
2. Review build reports
3. Verify schedule settings
4. Test manual triggers

---

**Happy Building! 🚀**
