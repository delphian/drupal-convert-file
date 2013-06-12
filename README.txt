Please see this original document at https://github.com/delphian/drupal-convert-file/wiki

__Drupal Convert File__ is a Drupal 7 module to automatically or manually convert existing or newly uploaded files into different formats. Conversion is done with a plugin system; "providers" each contribute various "formats" to which a file may be converted. The core module comes with providers: Google Drive, and ImageMagick. Two widgets will be installed: Convert File for file field types, and Convert Image for image field types. Field settings will have two [new drop down options](https://github.com/delphian/drupal-convert-file/wiki/Features#automatic-conversion-of-uploaded-files). The first selects the provider that will deal with transformation, the second specifies what format the provider should convert to.

#### Features ####

* [Inline conversion of existing files](https://github.com/delphian/drupal-convert-file/wiki/Features#inline-conversion-of-existing-files)
* [Automatic conversion of uploaded files](https://github.com/delphian/drupal-convert-file/wiki/Features#automatic-conversion-of-uploaded-files)
* [Custom field formatters](https://github.com/delphian/drupal-convert-file/wiki/Features#field-formatters)
* [File conversion based on rules](https://github.com/delphian/drupal-convert-file/wiki/Features#file-conversion-is-based-on-rules)
* [Programmatic file conversion](https://github.com/delphian/drupal-convert-file/wiki/Features#programmatic-file-conversion)
* [File conversion done inside validators](https://github.com/delphian/drupal-convert-file/wiki/Features#file-conversion-done-inside-validators)

#### Requirements ####

* Drupal modules: Rules, Entity tokens, Entity API
* cURL PHP extension

#### Installation ####

1. Clone repository into sites/all/modules/custom/ `git clone --recursive git://github.com/delphian/drupal-convert-file.git`.
2. Enable modules `drush en convertfile cf_googledrive cf_convertfile cf_imagemagick -y`.
3. Configure providers [Google Drive](https://github.com/delphian/drupal-convert-file/wiki/Google-Drive) or [ImageMagick](https://github.com/delphian/drupal-convert-file/wiki/ImageMagick).
4. [Assign Convert File widget to file field](https://github.com/delphian/drupal-convert-file/wiki/Assign-Convert-File-Widget-to-File-Field).

#### Documentation & Help ####

* [Admin Interface](https://github.com/delphian/drupal-convert-file/wiki/Admin-Interface)
* [Developer Guide](https://github.com/delphian/drupal-convert-file/wiki/Developer-Guide).
* [Issue Queue](https://github.com/delphian/drupal-convert-file/issues/new).
* Providers
  * [Convert File](https://github.com/delphian/drupal-convert-file/wiki/Convert-File) _.gz_
  * [Google Drive](https://github.com/delphian/drupal-convert-file/wiki/Google-Drive) _.docx .odt .pdf .txt_
  * [ImageMagick](https://github.com/delphian/drupal-convert-file/wiki/ImageMagick) _.jpg .png .pdf_

Feel free to contact me bryan.hazelbaker@gmail.com with any questions.
