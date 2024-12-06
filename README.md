# AMSmartProperties - Library for solving functional dependencies.

Smart properties is easy wa for making robust code, when, you need right value in some variable
that depends on another variables. Simply, it's easy wa for making reponsive GUIs and many other
applications. More [Smart Properties](http://libandromeda.org/amsmartproperties/latest/group__SmartProperties.html).

## Debugging

Debugging facilities for Smart properties and general use. More [Debug](http://libandromeda.org/amsmartproperties/latest/group__Debug.html).

## Tutorials

[Smart Properties Tutorial](http://libandromeda.org/amsmartproperties/latest/SmartPropertiesTutorial.html)

[Smart Properties Tutoriál (CS)](http://libandromeda.org/amsmartproperties/latest/SmartPropertiesTutorial_cs.html)

## Examples

```bash
; go to root of the project
cd examples
make
```

## Documetation

There are doxygen generated documentation [here on libandromeda.org](http://libandromeda.org/amsmartproperties/latest/).

## Building AMLString

### Getting sources

```bash
git submodule update
```

### Compiling

```bash

mkdir cmake-build-debug

cd cmake-build-debug

cmake ..

make
```

### Output Library

```bash
/lib/libAMSmartProperties.so
```

### Single test (not necessary)

```bash
./TEST_AMFuntion

./TEST_AMProperties
```

## License

This library is under GNU GPL v3 license. If you need business license, don't hesitate to contact [me](mailto:zdenek.skulinek\@robotea.com\?subject\=License%20for%20AMSmartProperties).

## Contribute

Please contact [me](mailto:zdenek.skulinek\@robotea.com\?subject\=License%20for%20AMSmartProperties).

## Ask for help

Please contact [me](mailto:zdenek.skulinek\@robotea.com\?subject\=Consultation).

## Dependencies

- [Google test](https://github.com/google/googletest.git)
- [AMFNV1a hash algorithm](https://github.com/robotea/amfnv1a.git)
- [AMLString localized strings](https://github.com/robotea/amlstring.git)
- [AMException cascaded exception](https://github.com/robotea/amexception.git)
