ioeX BravoMesh Carrier Module Android SDK
===========================
## Summary

ioeX BravoMesh Carrier Module Android SDK is java api wrapper for ioeX BravoMesh Carrier Module, where Carrier Module is a decentralized peer to peer communication framework.

## Build from source

### 1.Build Carrier Module NDK

You need to build carrier module android ndk distributions from the Carrier module repository with following github address.

```
https://github.com/ioeXNetwork/ioeX.BravoMesh.CarrierModule
```

Finished building android ndk for Carrier Module, you would have native output library **libcarrier-native.a** to each CPU arch, currently supported for **armv7l**, **arm64**, **x86**, **x86-64**.

### 2.Import Carrier Module NDK

The directory **"app/native-dist"** to import native libraries and headers should have following directory structure:

```
app/native-dist
   |--include
       |--IOEX_carrier.h
       |--IOEX_session.h
   |--libs
       |--armeabi
          |--libcarrier-native.a
       |--armeabi-v7a
          |--libcarrier-native.a
       |--arm64-v8a
          |--libcarrier-native.a
       |--x86
          |--libcarrier-native.a
       |--x86-64
          |--libcarrier-native.a
```

The headers under subdirectory **"include"** are public header files exported from Carrier native. And **"libcarrier-native.a"** is just a static library dist to each CPU arch from Carrier native.

### 3. Build Carrier Module SDK

After importing dependencies from Carrier Module, you need Android Studio to open this project and build Carrier Module Android SDK.

### 4. Output

After building with success, the output dist named **org.ioex.carrier-debug(release).aar** carrying jar package and JNI shared libraries to different CPU arch would be put under the directory:

```
app/build/outputs/aar
```

## Basic Tests

All basic tests are located under directory **"app/src/androidTest"**. You can run the tests on Android Studio. Before running tests, you need to uncomment **"service"** configuration in AndroidMinifest.xml.

## Build Docs

Open **Tools** tab on Android Studio and click **Generate JavaDoc...** item to generate the Java API document.

## Thanks

Sincerely thanks to all teams and projects that we relies on directly or indirectly.

## Contributing

We welcome contributions to the ioeX BravoMesh Carrier Module Android Project (or Native Project) in many forms.

## License

ioeX BravoMesh Carrier Module Android Project source code files are made available under the MIT License, located in the LICENSE file. 