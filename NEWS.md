2025-02-08 v2025-02
--------------------

### Bug fixes

* Fixed build crash when using Qt version 5.6.
* Fixed cursor movement when pressing "replace and find next" button.
* Added undo/redo for text replacement operations.

### New features

Added settings that affect opening and saving files:

* File encoding choices. Some CNC systems allow to use non-Latin characters in
  comments, which are located in the second half of the
  [code page](https://en.wikipedia.org/wiki/Code_page#Windows_code_pages).
  Choosing the encoding allows you to display such characters correctly. The
  system encoding is used by default.
* Remove characters from the second half of the codepage when opening files.
  This option is disabled by default.
* Removing control characters when opening files. Older NC systems, when
  transmitting through the serial port, could add additional control characters
  that the editor cannot display correctly. The option allows you to delete
  control characters other than carriage and newline characters. This option
  is enabled by default.
* Deleting additional blank lines. Some CNC may insert blank lines after each
  program line when writing to removable media. The option allows you to
  recognize and delete such additional blank lines. This option is enabled by
  default.

The new settings are available on the "Open/Save" tab in the configuration
dialog box.


2024-10-21 v2024-10
--------------------

### Bug fixes

* Fixed a file saving bug where reducing an editable program did not reduce the
  file on disk.
* Fixed calculation of required disk space for .deb packages.


2024-09-27 v2024-09
--------------------

### Bug fixes

* Fixed program crash when trying to compare files containing tab characters.
* Fixed minor bugs.

### Updates

* German translations have been updated, thanks to Michael Numberger.

### New features

* Using GitHub Actions, binary builds for Debian, Ubuntu and Windows are
  available on the release page.



2024-05-30 v2024-05
--------------------

Fork renamed to avoid confusion.

### New features

* Opening a file by dragging and dropping.
* Prohibit clipboard browsing when application window is inactive.
* Installer for Windows.
