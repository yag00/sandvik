/*
 * This file is part of Sandvik project.
 * Copyright (C) 2025 Christophe Duvernois
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "apk.hpp"

#include <axml/axml_parser.h>

#include <LIEF/DEX/Class.hpp>
#include <LIEF/DEX/File.hpp>
#include <LIEF/DEX/Method.hpp>
#include <regex>
#include <string>
#include <vector>

#include "fmt/format.h"

#include "class.hpp"
#include "dex.hpp"
#include "field.hpp"
#include "method.hpp"
#include "system/logger.hpp"
#include "system/zip.hpp"

using namespace sandvik;

/** Constructor: Loads the JAR file */
Apk::Apk(const std::string& path_, Dex& classes_dex_) : _path(path_), _classes_dex(classes_dex_) {
	if (!ZipReader::isValidArchive(_path)) {
		throw std::runtime_error(fmt::format("Invalid APK file: {}", _path));
	}

	_zipReader = std::make_unique<ZipReader>();
	_zipReader->open(_path);
	std::list<std::string> files = _zipReader->getList();

	// load classes.dex
	std::string file = "classes.dex";
	uint64_t size = 0;
	char* buffer = _zipReader->extractToMemory(file, size);
	if (buffer == nullptr) {
		throw std::runtime_error(fmt::format("Failed to extract {}", file));
	}
	std::vector<uint8_t> dexBuffer(buffer, buffer + size);
	free(buffer);
	_classes_dex.load(dexBuffer, file);

	// load AndroidManifest.xml
	file = "AndroidManifest.xml";
	size = 0;
	buffer = _zipReader->extractToMemory(file, size);
	if (buffer == nullptr) {
		throw std::runtime_error(fmt::format("Failed to extract {}", file));
	}
	// Convert to string and search for launcher activity
	_manifest = std::string(buffer, size);
	free(buffer);
	_mainActivity = findMainActivity();
}

Apk::~Apk() {
	_zipReader->close();
}

std::string Apk::getPath() const {
	return _path;
}
std::string Apk::getMainActivity() const {
	return _mainActivity;
}

std::vector<std::string> Apk::getClassNames() const {
	std::vector<std::string> classNames;
	for (const auto& classname : _classes_dex.getClassNames()) {
		classNames.push_back(classname);
	}
	return classNames;
}

std::string Apk::findMainActivity() const {
	// Parse the AXML
	try {
		axml::AXMLFile axmlFile((void*)_manifest.data(), _manifest.size());
		axml::AXMLParser parser(axmlFile);

		std::string packageName;
		std::string mainActivity;
		while (parser.next()) {
			switch (parser.eventType()) {
				case axml::AXMLParser::START_ELEMENT: {
					std::string elementName = parser.getElementName();

					if (elementName == "manifest") {
						// Get package name from manifest
						for (size_t i = 0; i < parser.getElementAttributeCount(); i++) {
							if (parser.getElementAttributeName(i) == "package") {
								packageName = parser.getElementAttributeRawValue(i);
								break;
							}
						}
					} else if (elementName == "activity") {
						std::string activityName;
						bool hasMainAction = false;
						bool hasLauncherCategory = false;

						// Get activity name
						for (size_t i = 0; i < parser.getElementAttributeCount(); i++) {
							if (parser.getElementAttributeName(i) == "name") {
								activityName = parser.getElementAttributeRawValue(i);
								break;
							}
						}

						// Check child intent-filter elements
						while (parser.next()) {
							if (parser.eventType() == axml::AXMLParser::END_ELEMENT && parser.getElementName() == "activity") {
								break;
							}

							if (parser.eventType() == axml::AXMLParser::START_ELEMENT && parser.getElementName() == "intent-filter") {
								// Check for MAIN action and LAUNCHER category
								while (parser.next()) {
									if (parser.eventType() == axml::AXMLParser::END_ELEMENT && parser.getElementName() == "intent-filter") {
										break;
									}

									if (parser.eventType() == axml::AXMLParser::START_ELEMENT) {
										if (parser.getElementName() == "action") {
											for (size_t i = 0; i < parser.getElementAttributeCount(); i++) {
												if (parser.getElementAttributeName(i) == "name" &&
												    parser.getElementAttributeRawValue(i) == "android.intent.action.MAIN") {
													hasMainAction = true;
												}
											}
										} else if (parser.getElementName() == "category") {
											for (size_t i = 0; i < parser.getElementAttributeCount(); i++) {
												if (parser.getElementAttributeName(i) == "name" &&
												    parser.getElementAttributeRawValue(i) == "android.intent.category.LAUNCHER") {
													hasLauncherCategory = true;
												}
											}
										}
									}
								}

								if (hasMainAction && hasLauncherCategory && !activityName.empty()) {
									// Resolve full class name
									if (activityName[0] == '.') {
										// Relative path - prepend package name
										mainActivity = packageName + activityName;
									} else if (activityName.find('.') == std::string::npos) {
										// Simple name - prepend package name
										mainActivity = packageName + "." + activityName;
									} else {
										// Already fully qualified
										mainActivity = activityName;
									}

									// Convert to Java class format (replace . with /)
									std::replace(mainActivity.begin(), mainActivity.end(), '.', '/');
									return "L" + mainActivity + ";";
								}
							}
						}
					}
					break;
				}

				case axml::AXMLParser::END_ELEMENT: {
					break;
				}

				default:
					break;
			}
		}

		return mainActivity;
	} catch (const std::exception& e) {
		logger.error(fmt::format("AXML parsing failed: {}", e.what()));
		return "";
	}
}

std::unique_ptr<Class> Apk::findClass(ClassLoader& classloader_, const std::string& name) const {
	return _classes_dex.findClass(classloader_, name);
}