/**
 * Test Runner for AfterburnerControl React Native App
 * 
 * This script provides utilities for running tests and generating reports
 */

import { execSync } from 'child_process';
import { existsSync, mkdirSync } from 'fs';
import { join } from 'path';

class TestRunner {
  private testResultsDir = join(__dirname, '../../../test-results');
  private coverageDir = join(__dirname, '../../../coverage');

  constructor() {
    this.setupDirectories();
  }

  private setupDirectories(): void {
    if (!existsSync(this.testResultsDir)) {
      mkdirSync(this.testResultsDir, { recursive: true });
    }
    if (!existsSync(this.coverageDir)) {
      mkdirSync(this.coverageDir, { recursive: true });
    }
  }

  /**
   * Run all tests
   */
  public runAllTests(): void {
    console.log('🧪 Running all tests...');
    
    try {
      execSync('npm test -- --coverage --watchAll=false --verbose', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ All tests passed!');
    } catch (error) {
      console.error('❌ Some tests failed:', error);
      process.exit(1);
    }
  }

  /**
   * Run tests with coverage
   */
  public runTestsWithCoverage(): void {
    console.log('📊 Running tests with coverage...');
    
    try {
      execSync('npm test -- --coverage --watchAll=false --coverageReporters=text --coverageReporters=lcov', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ Coverage report generated!');
    } catch (error) {
      console.error('❌ Coverage test failed:', error);
      process.exit(1);
    }
  }

  /**
   * Run specific test file
   */
  public runTestFile(testFile: string): void {
    console.log(`🧪 Running test file: ${testFile}`);
    
    try {
      execSync(`npm test -- ${testFile} --watchAll=false`, {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ Test file passed!');
    } catch (error) {
      console.error('❌ Test file failed:', error);
      process.exit(1);
    }
  }

  /**
   * Run tests in watch mode
   */
  public runTestsInWatchMode(): void {
    console.log('👀 Running tests in watch mode...');
    
    try {
      execSync('npm test -- --watch', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
    } catch (error) {
      console.error('❌ Watch mode failed:', error);
      process.exit(1);
    }
  }

  /**
   * Generate test report
   */
  public generateTestReport(): void {
    console.log('📋 Generating test report...');
    
    try {
      execSync('npm test -- --coverage --watchAll=false --json --outputFile=test-results/results.json', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ Test report generated!');
    } catch (error) {
      console.error('❌ Test report generation failed:', error);
    }
  }

  /**
   * Run linting
   */
  public runLinting(): void {
    console.log('🔍 Running linting...');
    
    try {
      execSync('npm run lint', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ Linting passed!');
    } catch (error) {
      console.error('❌ Linting failed:', error);
      process.exit(1);
    }
  }

  /**
   * Run type checking
   */
  public runTypeChecking(): void {
    console.log('🔍 Running type checking...');
    
    try {
      execSync('npx tsc --noEmit', {
        stdio: 'inherit',
        cwd: join(__dirname, '../../..'),
      });
      console.log('✅ Type checking passed!');
    } catch (error) {
      console.error('❌ Type checking failed:', error);
      process.exit(1);
    }
  }

  /**
   * Run all quality checks
   */
  public runQualityChecks(): void {
    console.log('🔍 Running all quality checks...');
    
    this.runLinting();
    this.runTypeChecking();
    this.runAllTests();
    
    console.log('✅ All quality checks passed!');
  }
}

// Export the TestRunner class
export default TestRunner;

// If this file is run directly, execute the appropriate command
if (require.main === module) {
  const runner = new TestRunner();
  const command = process.argv[2];
  
  switch (command) {
    case 'all':
      runner.runAllTests();
      break;
    case 'coverage':
      runner.runTestsWithCoverage();
      break;
    case 'watch':
      runner.runTestsInWatchMode();
      break;
    case 'report':
      runner.generateTestReport();
      break;
    case 'lint':
      runner.runLinting();
      break;
    case 'types':
      runner.runTypeChecking();
      break;
    case 'quality':
      runner.runQualityChecks();
      break;
    default:
      if (command && command.endsWith('.test.ts') || command.endsWith('.test.tsx')) {
        runner.runTestFile(command);
      } else {
        console.log(`
Usage: npm run test:runner <command>

Commands:
  all       - Run all tests
  coverage  - Run tests with coverage
  watch     - Run tests in watch mode
  report    - Generate test report
  lint      - Run linting
  types     - Run type checking
  quality   - Run all quality checks
  <file>    - Run specific test file

Examples:
  npm run test:runner all
  npm run test:runner coverage
  npm run test:runner components/ColorInput.test.tsx
        `);
      }
  }
}
