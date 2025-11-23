# Contributing to HeromanGame

Thanks for your interest in contributing to HeromanGame! We welcome contributions from the community.

## How to Submit Changes

1. **Fork this repository** - Click the "Fork" button at the top right of this page
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR-USERNAME/HeromanGame.git
   ```
3. **Create a new branch** for your feature or fix:
   ```bash
   git checkout -b feature/my-feature
   ```
4. **Make your changes** - Edit the code as needed
5. **Test your changes** - Build and test with PlatformIO:
   ```bash
   pio run
   pio run --target upload  # Test on hardware if possible
   ```
6. **Commit your changes** with a clear, descriptive message:
   ```bash
   git commit -m 'Add feature: description of what you added'
   ```
7. **Push to your fork**:
   ```bash
   git push origin feature/my-feature
   ```
8. **Open a Pull Request** - Go to the original repository and click "New Pull Request"

## Code Standards

- Follow the existing code style and structure
- Comment your code where the logic isn't immediately obvious
- Keep commits focused - one feature or fix per commit when possible
- Test on actual hardware if you have access to ESP32 devices

## Types of Contributions

We welcome:
- Bug fixes
- New features for the game
- Performance improvements
- Documentation improvements
- Code cleanup and refactoring

## Reporting Issues

If you find a bug or have a feature request:
1. Check if an issue already exists
2. If not, open a new issue with:
   - Clear description of the problem or feature
   - Steps to reproduce (for bugs)
   - Expected vs actual behavior
   - Hardware/platform information

## Questions?

Feel free to open an issue for questions or reach out to the maintainers!

## License

By contributing, you agree that your contributions will be licensed under the same license as this project.
