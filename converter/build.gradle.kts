plugins {
    alias(libs.plugins.kotlin)
    alias(libs.plugins.kotlinter)
    alias(libs.plugins.shadow)
    alias(libs.plugins.detekt)
    alias(libs.plugins.versions)
    alias(libs.plugins.dependency.analysis)
    application
}

group = "net.chrissearle"
version = "1.0-SNAPSHOT"
description = "converter"

application {
    mainClass.set("net.chrissearle.converter.ConvertKt")
}

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

tasks.shadowJar {
    archiveFileName.set("converter.jar")
}

tasks.jar {
    enabled = false
}
