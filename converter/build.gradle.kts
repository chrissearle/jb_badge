plugins {
    alias(libs.plugins.kotlin)
    alias(libs.plugins.kotlinter)
    alias(libs.plugins.detekt)
    alias(libs.plugins.graal)
    alias(libs.plugins.versions)
    alias(libs.plugins.dependency.analysis)
}

group = "net.chrissearle"
version = "1.0-SNAPSHOT"
description = "converter"

kotlin {
    jvmToolchain(22)

    compilerOptions {
        freeCompilerArgs = listOf("-Xconsistent-data-class-copy-visibility")
    }
}

repositories {
    mavenCentral()
}

dependencies {
    implementation(libs.guava)
}

graalvmNative {
    binaries {
        named("main") {
            imageName.set("jb_badge_converter")
            mainClass.set("net.chrissearle.converter.ConvertKt")
            javaLauncher.set(
                javaToolchains.launcherFor {
                    languageVersion.set(JavaLanguageVersion.of(22))
                    vendor.set(JvmVendorSpec.GRAAL_VM)
                },
            )
        }
    }
}
