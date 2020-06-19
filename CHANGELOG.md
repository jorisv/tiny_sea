# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- Add **fPIC** option to the conan recipe

## [0.3.0] - 2020-06-05
### Added
- Add simple iterator **CloseList**, **BinaryHeapOpenList** and **BinaryHeapNUOpenList**.
- Add few getter to **CloseList**, **BinaryHeapOpenList** and **BinaryHeapNUOpenList**.

### Fixed
- Fix how **StateFactory** compute the g cost.

## [0.2.0] - 2020-06-01
### Added
- **BinaryHeapOpenList** OpenList implementation based on a hashmap and a binary heap.
- **BinaryHeapNUOpenList** OpenList implementation only based on a hashmap.
