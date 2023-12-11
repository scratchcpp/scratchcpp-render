<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <!-- TODO: Add logo: <a href="https://github.com/scratchcpp/scratchcpp-gui">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a> -->

<h3 align="center">ScratchCPP GUI</h3>

  <p align="center">
    A Qt-based library which uses libscratchcpp and QNanoPainter to render running Scratch projects.
    <br />
    <!-- TODO: Add link to documentation: <a href="https://github.com/scratchcpp/scratchcpp-gui"><strong>Explore the docs »</strong></a>
    <br /> -->
    <br />
    <a href="https://github.com/scratchcpp/scratchcpp-gui/issues">Report Bug</a>
    ·
    <a href="https://github.com/scratchcpp/scratchcpp-gui/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

**scratchcpp-gui** is a library which provides a set of Qt Quick components that can be used to run Scratch projects.

It uses **QNanoPainter** on top of Qt for hardware accelerated rendering
and **libscratchcpp** to read, run and maintain the state of Scratch projects.

## NOTE
This library is still in development and it shouldn't be used in production yet.

**There might be incompatible API changes anytime before version 1.0.0 releases!**

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

We're working on the documentation, it'll be available soon.
<!-- TODO: Add link to documentation -->

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage
The most common way to get started is to use the `ProjectPlayer` component.
```qml
import QtQuick
import ScratchCPPGui

Window {
    width: 480
    height: 360
	visible: true
	title: qsTr("Hello World")

    ProjectPlayer {
        id: player
        fileName: "/path/to/project.sb3"
        onLoaded: start()
    }
}
```
The project will start when the window opens.
If you would like to have a "green flag" button instead, call `player.start()` from the button:
```qml
Button {
    onClicked: player.start()
}
```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] JPEG and PNG rendering
- [x] Everything related to displaying sprites (position, rotation, size, etc.)
- [x] Loading projects in another thread
- [x] API for engine properties (FPS, turbo mode, etc.)
- [x] Loading projects from URL
- [x] SVG rendering
- [ ] Mouse position
- [ ] Key press events
- [ ] Mouse press events
- [ ] Sprite click events
- [ ] Clones
- [ ] Sprite dragging
- [ ] Touching color blocks
- [ ] Pen blocks
- [ ] Monitors
- [ ] Graphics effects (maybe using shaders)
- [ ] Speech and thought bubbles
- [ ] Question text box ("ask and wait" block)

See the [open issues](https://github.com/scratchcpp/scratchcpp-gui/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Format changed source files (`clang-format src/some_file.cpp`)
4. Commit your Changes (`git add -A && git commit -m 'Add some AmazingFeature'`)
5. Push to the Branch (`git push origin feature/AmazingFeature`)
6. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the GNU Lesser General Public License. See [LICENSE](LICENSE) for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/scratchcpp/scratchcpp-gui.svg?style=for-the-badge
[contributors-url]: https://github.com/scratchcpp/scratchcpp-gui/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/scratchcpp/scratchcpp-gui.svg?style=for-the-badge
[forks-url]: https://github.com/scratchcpp/scratchcpp-gui/network/members
[stars-shield]: https://img.shields.io/github/stars/scratchcpp/scratchcpp-gui.svg?style=for-the-badge
[stars-url]: https://github.com/scratchcpp/scratchcpp-gui/stargazers
[issues-shield]: https://img.shields.io/github/issues/scratchcpp/scratchcpp-gui.svg?style=for-the-badge
[issues-url]: https://github.com/scratchcpp/scratchcpp-gui/issues
[license-shield]: https://img.shields.io/github/license/scratchcpp/scratchcpp-gui.svg?style=for-the-badge
[license-url]: https://github.com/scratchcpp/scratchcpp-gui/blob/master/LICENSE
