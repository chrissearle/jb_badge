plugins {
    alias(libs.plugins.kotlin)
    alias(libs.plugins.kotlinter)
    alias(libs.plugins.shadow)
    alias(libs.plugins.detekt)
    alias(libs.plugins.versions)
    application
}

group = "net.chrissearle"
version = "1.0-SNAPSHOT"
description = "converter"

application {
    mainClass.set("net.chrissearle.converter.ConvertKt")
}

kotlin {
    jvmToolchain(21)
}

repositories {
    mavenCentral()
}

dependencies {
    implementation(libs.guava)
}

tasks.withType<org.jetbrains.kotlin.gradle.tasks.KotlinCompile> {
    kotlinOptions {
        freeCompilerArgs = listOf("-Xjsr305=strict")
    }
}

tasks.shadowJar {
    archiveFileName.set("converter.jar")
}

tasks.jar {
    enabled = false
}
