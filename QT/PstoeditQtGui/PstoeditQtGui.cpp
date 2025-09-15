/*
   PstoeditQtGUI.cpp : This file is part of pstoedit. Implementation of the QT GUI.
  
   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "PstoeditQtGui.h"
#include <QSpinBox>
#include <QCheckBox>
#include <QScrollArea>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QPushButton>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <sstream>
#include <string.h> 
#include <filesystem>
#include "miscutil.h"

static constexpr char separatorInFormalList = ';';

static QString GetValueOfOption(const OptionBase* theOption) {
	// using streams because returning a std:string from the DLL
	// causes issues with memory management.
	C_ostrstream tempstream;
	(void)theOption->writevalue(tempstream);
	tempstream << std::ends;
	const QString curvalue(tempstream.str().data());
	return curvalue;
	//	logStream << "results in '" << curvalue.toStdString().c_str() << "'" << endl;
}

PstoeditQtGui::PstoeditQtGui(int argc_p, char ** argv_p, std::ostream& logStream_p, std::ostream* debugStream_p, QWidget* parent)
	: QMainWindow(parent), logStream(logStream_p), debugStream(debugStream_p), argc(argc_p), argv(argv_p)
{
	ui.setupUi(this);
	ui.logWidget->setReadOnly(true);
	QObject::connect(ui.actionQuit, &QAction::triggered, this, &PstoeditQtGui::quit);
	QObject::connect(ui.actionSelectFiles, &QAction::triggered, this, &PstoeditQtGui::getInputFiles);
	QObject::connect(ui.actionOpen_Help_Manual, &QAction::triggered, this, &PstoeditQtGui::OpenGUIHelpDocument);
	QObject::connect(ui.actionCheck_for_Updates, &QAction::triggered, this, &PstoeditQtGui::CheckForUpdates);
	QObject::connect(ui.actionGet_Support_or_open_a_Ticket, &QAction::triggered, this, &PstoeditQtGui::Get_Support_or_open_a_Ticket);
	QObject::connect(ui.actionSupport_Pstoedit_Development_and_Maintenance, &QAction::triggered, this, &PstoeditQtGui::SupportPstoeditDevelopmentandMaintenance);
	QObject::connect(ui.actionAbout, &QAction::triggered, this, &PstoeditQtGui::About);
	QObject::connect(ui.actionAbout_Qt, &QAction::triggered, this, &QApplication::aboutQt);

	std::filesystem::path abs_path;
	const char* docdir = getPstoeditDocDirectory();
	if (docdir) {
		abs_path = std::filesystem::absolute(docdir); // for Linux - docdir is null otherwise
	} else {
		char * path_to_exe = cppstrdup(argv[0]);
		char* lastSlash = nullptr;
		// replace \\ with / and find last /
		for (char* cp = path_to_exe; *cp; cp++) {
			if (*cp == '\\') *cp = '/';
			if (*cp == '/') lastSlash = cp;
		}
		if (lastSlash) {
			*lastSlash = '\0';
		}
		if (path_to_exe && strlen(path_to_exe)) {
		  abs_path = std::filesystem::absolute(path_to_exe);
		} else {
		  abs_path = std::filesystem::current_path(); 
		}
		delete [] path_to_exe;
	}
	UrlOfPstoeditHtml = QString("file:///");
#if defined(_WIN32) || defined(_GLIBCXX_FILESYSTEM_IS_WINDOWS) 
	UrlOfPstoeditHtml += QString::fromStdWString(abs_path.native());
#else
	UrlOfPstoeditHtml += QString(abs_path.native().c_str());
#endif
	UrlOfPstoeditHtml += QString("/pstoedit.htm");
	UrlOfPstoeditHtml.replace(QString("\\"), QString("/"));
}

void PstoeditQtGui::fillTabWithOptions(int new_tab_index, ProgramOptions* options,  int sheet, unsigned int opttype, QFormLayout* layout, const char* anchorprefix)
{
	QWidget* cur_sheet = ui.tabWidget->widget(new_tab_index);
	if (cur_sheet) {
		const unsigned int nrOfOptions = options->numberOfOptions();
		//	logStream << "nr of options: " << nrOfOptions << endl;
		for (unsigned int i = 0; i < nrOfOptions; i++) {
			OptionBase* iterator = options->iThOption(i);
			assert(iterator);
			if (iterator->hideInGui) continue;
			const unsigned int curtype = iterator->gettypeID();
			// const char* curtypename = iterator->gettypename();
			// sheet -1 is for driver specific options where all options are on one page
			if ((curtype == opttype) && ((sheet < 0) || (iterator->propsheet == (unsigned int) sheet))
				) {
				//QString label((iterator)->argname /*description*/ );
				QString labelString;
#if 0
				labelString += "(";
				labelString += iterator->argname; // might be ""
				labelString += ") ";
#endif
				labelString += iterator->description;
				labelString += " (<a href=\"";
				labelString += UrlOfPstoeditHtml;
				labelString += "#option-";
				labelString += anchorprefix;
				labelString += iterator->flag;
				labelString += "\">";
				labelString += iterator->flag;
				labelString += "</a>)";
				QLabel* label = new QLabel(labelString);
				label->setTextFormat(Qt::RichText);
				label->setTextInteractionFlags(Qt::TextBrowserInteraction);
				label->setOpenExternalLinks(true);

				const QString curvalue(GetValueOfOption(iterator));

				// special handling of some options
				if (getPstoeditsetDLLUsage() &&
					(std::string(iterator->flag) == "-gs") &&
				    (curvalue == "") 
				   ) {
					// not specified yet - so create selector box
					versionSelector = new QComboBox(nullptr);
					versionSelector->addItems(versionSelectorItems);
					widgetsToOptions.push_back(new ComboBoxToOption(versionSelector, iterator, new_tab_index));
					layout->addRow(versionSelector, label);
				} else {
					switch (curtype) {
					case bool_ty: {
						QCheckBox* checkBox = new QCheckBox();
						widgetsToOptions.push_back(new CheckboxToOption(checkBox, iterator, new_tab_index));
						checkBox->setChecked((curvalue == "1"));
						layout->addRow(checkBox, label);
					}
								break;
					case int_ty: {
						QLineEdit* inputBox = new QLineEdit(curvalue, nullptr);
						inputBox->setValidator(new QIntValidator());
						widgetsToOptions.push_back(new LineEditToOption(inputBox, iterator, new_tab_index));
						layout->addRow(inputBox, label);
					}
							   break;
					case double_ty: {
						QLineEdit* inputBox = new QLineEdit(curvalue, nullptr);
						inputBox->setValidator(new QDoubleValidator());
						widgetsToOptions.push_back(new LineEditToOption(inputBox, iterator, new_tab_index));
						layout->addRow(inputBox, label);
					}
								  break;
					case char_ty: {
						QLineEdit* inputBox = new QLineEdit(curvalue, nullptr);
						inputBox->setInputMask(QString("X"));
						widgetsToOptions.push_back(new LineEditToOption(inputBox, iterator, new_tab_index));
						layout->addRow(inputBox, label);
					}
								break;
					case rsstring_ty:
					case constcharstring_ty: {
						QLineEdit* inputBox = new QLineEdit(curvalue, nullptr);
						widgetsToOptions.push_back(new LineEditToOption(inputBox, iterator, new_tab_index));
						layout->addRow(inputBox, label);
					}
										   break;
					case argv_ty: {
						QLineEdit* inputBox = new QLineEdit(curvalue, nullptr);
						widgetsToOptions.push_back(new LineEditToOption(inputBox, iterator, new_tab_index));
						layout->addRow(inputBox, label);
					}
								break;
					default:
						assert(false); // should not come here
					}
				}
			}
		}
	}
}

void PstoeditQtGui::transferOptionValues() {
	for (const auto& binder : widgetsToOptions) {
		// logStream << "transferring value for " << binder->theOption->flag << endl;
		binder->transferValue();
	}
}

void PstoeditQtGui::removeTabFromMappingTable(int tab_to_be_removed) {
	widgetsToOptions.remove_if([tab_to_be_removed](WidgetToOptionBase* item) { return item->tab_index == tab_to_be_removed; });
}

int PstoeditQtGui::createOptionTab(ProgramOptions* options, int sheet, const char * const sheetName, const char* anchorprefix) {
	QWidget* new_tab = new QWidget();
	new_tab->setObjectName(sheetName);
	const int new_tab_index = ui.tabWidget->addTab(new_tab, QString(sheetName));

	QWidget* content_widget = new QWidget(new_tab);
	QFormLayout* layout = new QFormLayout(content_widget);

	// write the non bool_t first
	for (unsigned int type_id = int_ty; type_id <= argv_ty; type_id++) {
		if (type_id != bool_ty) {
			fillTabWithOptions(new_tab_index, options, sheet, type_id, layout, anchorprefix);
		}
	}
	// now write the bool_t  
	fillTabWithOptions(new_tab_index, options, sheet, bool_ty, layout, anchorprefix);

	const QString scrolllabel(QString("ScrollArea for: ") + QString::number(sheet));
	QScrollArea* scrollArea = new QScrollArea();
	scrollArea->setObjectName(scrolllabel);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(content_widget);

	QVBoxLayout* boxLayout = new QVBoxLayout();
	boxLayout->addWidget(scrollArea);
	new_tab->setLayout(boxLayout);
	return new_tab_index;
}


void PstoeditQtGui::mapoptionstodialog() {
	unsigned int sheet = 0; // iterates over all "categories" (or "option sheets") in pstoeditoptions
	while (const char * sheetName = globaloptions->propSheetName(sheet)) {
		if (!globaloptions->hideSheetFromGui(sheet)) {
			createOptionTab(globaloptions, sheet, sheetName, "main");
		}
		sheet++;
	}

	// "patch" the version selector input to provide the gs versions
	// found.

	// box for format selection
	const DriverDescription_S* ptr = getPstoeditDriverInfo_plainC();
	const DriverDescription_S* driverInfoIterator = ptr;
	formatSelector = new QComboBox(ui.tab_ifo);
	int currentGroup = 1;
	while (driverInfoIterator && driverInfoIterator->symbolicname) {
		if (driverInfoIterator->formatGroup != currentGroup) {
			currentGroup++;
		    formatSelector->insertSeparator(INT32_MAX); // at end
		} 
		QString listentry(driverInfoIterator->symbolicname);
		listentry += separatorInFormalList;
		listentry += " ";
		listentry += driverInfoIterator->explanation;
#if 0
		char buf[100];
		sprintf(buf, " %d", driverInfoIterator->formatGroup);
		listentry += buf;
#endif
		listentry += separatorInFormalList;
		listentry += " ";
		listentry += driverInfoIterator->suffix;
		formatSelector->addItem(listentry);
		if (strcmp(driverInfoIterator->symbolicname, "gs") == 0) {
			// add shortcuts for some GS high level formats
			// see: https://ghostscript.readthedocs.io/en/latest/VectorDevices.html
			//pdfwrite, ps2write, eps2write, txtwrite, xpswrite, docxwrite, pxlmono, pxlcolor.
			const char* drivernames[] = { "pdfwrite", "ps2write", "eps2write", "txtwrite", "xpswrite", "docxwrite", "pxlmono", "pxlcolor"};
			const char* suffixes[] =    { "pdf",      "ps",       "eps",       "txt",      "xps",      "docx",      "pxl",     "pxl" };
			constexpr int nrOfItems = sizeof(drivernames)/sizeof(const char *);
			for (int i = 0; i < nrOfItems; i++) {
				QString entry("gs:");
				entry += drivernames[i];
				entry += separatorInFormalList;
				entry += " ";
				entry += "Ghostscript's ";
				entry += drivernames[i];
				entry += " device";
				entry += separatorInFormalList;
				entry += " ";
				entry += suffixes[i];
				formatSelector->addItem(entry);
			}
		}
		driverInfoIterator++;
	}
	formatSelector->setCurrentIndex(1);
	{
		QHBoxLayout* layout = new QHBoxLayout();
		inputFilesOutput = new QLineEdit("select input file(s)");
		layout->addWidget(inputFilesOutput);
		QPushButton* inputFileButton = new QPushButton("...");
		layout->addWidget(inputFileButton);
		ui.formLayout->addRow("Input file(s)", layout);
		connect(inputFileButton, &QPushButton::released, this, &PstoeditQtGui::getInputFiles);
	}
	ui.formLayout->addRow("Target format", formatSelector);
	connect(formatSelector, &QComboBox::activated, this, &PstoeditQtGui::updateOutputSuffix);
	{
		QHBoxLayout* layout = new QHBoxLayout();
		outputSuffix = new QLineEdit(".p2eout");
		layout->addWidget(outputSuffix);
		ui.formLayout->addRow("Suffix for generated files", layout);
	}
	startStopButton = new QPushButton("Start conversion");
	ui.formLayout->addRow(startStopButton);
	connect(startStopButton, &QPushButton::released, this, &PstoeditQtGui::startOrStopConversion);
	updateStartStopButton(ButtonState::FullyDisabled);

	QPushButton* quitButton = new QPushButton("Quit");
	ui.formLayout->addRow(quitButton);
	connect(quitButton, &QPushButton::clicked, this, &PstoeditQtGui::quit, Qt::QueuedConnection);
	ui.tabWidget->setCurrentWidget(ui.tab_ifo);

	updateOutputSuffix();
}

void PstoeditQtGui::quit() {
	QCoreApplication::quit();
}

void PstoeditQtGui::updateOutputSuffix() {
	const QString selectedFormat = formatSelector->currentText();
	char tmpbuf[100];
	std::strncpy(tmpbuf, selectedFormat.toStdString().c_str(), 99);
	char* colon2 = std::strrchr(tmpbuf, separatorInFormalList);
	colon2++; // skip blank
	*colon2 = '.';
	outputSuffix->setText(colon2);

	if (driverSpecificOptionTab) {
		ui.tabWidget->removeTab(driverSpecificOptionTab);
		// need to remove also from widget to options table.
		removeTabFromMappingTable(driverSpecificOptionTab);
		driverSpecificOptionTab = 0;
	}
	char* colon1 = std::strchr(tmpbuf, separatorInFormalList);
	*colon1 = '\0';// copy part up to first '|'
	const char* driverName = tmpbuf; // from start to :
	driverOptions = getProgramOptionsForDriver(driverName);
	// FIXME - delete former options
	
	if (driverOptions && driverOptions->numberOfOptions()) {
		//logStream << "The driver for this output format supports the following additional options: (specify using -f \"format:-option1 -option2\")" << endl;
		//driverOptions->showhelp(logStream, false, false);
		driverSpecificOptionTab = createOptionTab(driverOptions, -1 /* means show all types */, "Driver specific options", driverName);
	}
}

void PstoeditQtGui::getInputFiles() {
	files = QFileDialog::getOpenFileNames(
		this,
		"Select one or more files to convert",
		"",// Directory to look in
		"PostScript (*.ps *.eps);;All (*.*)");
	inputFilesOutput->setReadOnly(true);
	inputFilesOutput->setText(files.join(";"));
	if (!files.isEmpty()) {
		updateStartStopButton(ButtonState::StartEnabled);
	}
}

int PstoeditQtGui::runConversion() {
	return pstoeditwithghostscript(argc, argv, logStream, driverOptions, nullptr /* push_ins */);
}

void PstoeditQtGui::updateStartStopButton(ButtonState newState) {
	buttonState = newState;
	switch (buttonState) {
	case ButtonState::FullyDisabled:
		startStopButton->setEnabled(false);
		startStopButton->setText("Start/Cancel Conversion");
		break;
	case ButtonState::StartEnabled:
		startStopButton->setEnabled(true);
		startStopButton->setText("Start Conversion");
		break;
	case ButtonState::StopEnabled:
		startStopButton->setEnabled(true);
		startStopButton->setText("Cancel Conversion");
		break;
	default:
		assert(false);
	}
}

void PstoeditQtGui::startConversion() {
	updateStartStopButton(ButtonState::StopEnabled);

	continue_conversion = true;
	if (files.isEmpty()) {
		QMessageBox msgBox;
		msgBox.setText("You need to select file(s) to be converted.");
		msgBox.exec();
		return;
	}
	ui.tabWidget->setCurrentWidget(ui.tab_log);
	ui.logWidget->clear();
	ui.logWidget->focusWidget();

	this->show();
	this->update();
	
	transferOptionValues();
	OptionBase* verboseOption = globaloptions->optionByFlag("-v");
	verbose = (verboseOption && (GetValueOfOption(verboseOption) == "1"));

	OptionBase* formatOption = globaloptions->optionByFlag("-f");
	assert(formatOption);

	QString formatValue = GetValueOfOption(formatOption);
	//logStream << "format option from Dialog Box has value " << formatValue.toStdString().c_str() << endl;
	if (!formatValue.toStdString().length()) {
		const QString selectedFormat = formatSelector->currentText();
		//logStream << "format option from Combo Box has value " << selectedFormat.toStdString().c_str() << endl;
		// if not set inside the generic dialog text field use the Combo Box value
		char tmpbuf[100];
		std::strncpy(tmpbuf, selectedFormat.toStdString().c_str(), 99);
		char* colon = std::strchr(tmpbuf, separatorInFormalList);
		*colon = '\0';// copy part up to first '|'
		//logStream << "tmpbuf is '" << tmpbuf << "'" << endl;
		formatOption->copyValueFromString(tmpbuf);
	}

	if (verbose) {
		globaloptions->showvalues(logStream, true);
	}

	const int numfiles = files.length();
	QProgressDialog progress("Converting files...", "Cancel Conversion", 0, numfiles, this);
	progress.setWindowModality(Qt::WindowModal);
	int cnt = 0;
	for (const QString& infile : files) {
		progress.setValue(cnt);
		const QString outfile = infile + outputSuffix->text();
		const std::string outfile_string = outfile.toStdString();
		const std::string infile_string = infile.toStdString();
		globaloptions->setInputAndOutputFile(infile_string.c_str(), outfile_string.c_str());
		logStream << "Running conversion of  " << infile_string.c_str() << endl;
		future_for_conversion = QtConcurrent::run(&PstoeditQtGui::runConversion, this);
		const int res = future_for_conversion.result();
		logStream << "Finshed conversion of  " << infile_string.c_str() << " into " << outfile_string.c_str() << (res ? " with errors." : "") << endl;
		if (progress.wasCanceled() || !continue_conversion) {
			break;
		}
		cnt++;
	}
	progress.setValue(numfiles);
	updateStartStopButton(ButtonState::StartEnabled);
}

void PstoeditQtGui::cancelConversion() {
	assert(buttonState == ButtonState::StopEnabled);
	continue_conversion = false;
	future_for_conversion.cancel(); // I guess this is a no-op on a ::run job according to QT Documentation 
}

void PstoeditQtGui::startOrStopConversion() {
	assert(buttonState != ButtonState::FullyDisabled);
	if (buttonState == ButtonState::StartEnabled) {
		startConversion();
	} else {
		cancelConversion();
	}
}

bool PstoeditQtGui::checkGhostscript() {
	if (Verbose()) {
		cout << "ARGV[0]: " << argv[0] << endl;
	}
	if (getPstoeditsetDLLUsage()) {
		int count = 1; // first call does only counting
		(void)get_gs_versions(&count, nullptr, "" /* gsregbase*/, Verbose() /* verbose */);
		if (count) {
			int* versions = new int[count + 1];
			const char** versionstrings = new const char* [count + 1];
			versions[0] = count + 1;
			bool query_result = get_gs_versions(versions, versionstrings, "" /* gsregbase*/, Verbose() /* verbose */);
			if (query_result) {
				for (int i = 1; i <= count; i++) {
					gsversions.insert(std::pair < const int, std::string>(versions[i], versionstrings[i]));
					highestVersion = std::max(highestVersion, versions[i]);
				}
				if (Verbose()) std::cout << "The following versions of Ghostscript were found: " << endl;
				for (const auto& kv : gsversions) {
					if (Verbose()) std::cout << kv.first << " -> " << kv.second << std::endl;
					versionSelectorItems.push_front(QString(kv.second.c_str()));
					if (kv.first == highestVersion) highestVersionString = kv.second;
				}
			} else {
				cout << "something unexpected happened" << endl;
			}
			delete[] versions;
			delete[] versionstrings;
			return true;
		} else {
			cout << "Didn't find any installation of Ghostscript. Please install Ghostscript." << endl;
			return false;
		}
	} else {
		return true; // assume that we can call gs via command. Later FIXME: check command
	}
}

void PstoeditQtGui::setProgramOptions(ProgramOptions* param) {
	globaloptions = param;
	OptionBase* verboseOption = globaloptions->optionByFlag("-v");
	verbose = (verboseOption && (GetValueOfOption(verboseOption) == "1"));
}

void PstoeditQtGui::OpenGUIHelpDocument() const {
	QString helpfileURL = UrlOfPstoeditHtml;
	helpfileURL += QString("#GUI");
	QDesktopServices::openUrl(QUrl(helpfileURL, QUrl::TolerantMode));
}


void PstoeditQtGui::ReplyFinished(QNetworkReply* reply) {
	const QString answer = reply->readAll();
	const double latest_version = answer.toFloat();
	const double current_version = QString(get_pstoedit_version()).toFloat();
	if (Verbose()) {
		cout << "version.txt has: " << answer.toStdString().c_str() << ":" << latest_version << " your version is " << current_version << endl;
	}
	if (latest_version > current_version) {
		std::stringstream message;
		message << "New version " << answer.toStdString().c_str() << " is available. Your version is " << current_version << endl;
		message << "Get new version from https://sourceforge.net/projects/pstoedit/files/pstoedit/" << endl;
		QMessageBox::information(this, QString("Pstoedit"), message.str().c_str());
	} else if (latest_version < current_version) {
		std::stringstream message;
		message << "Your version " << current_version << " is newer than the released version " << answer.toStdString().c_str() << endl;
		QMessageBox::information(this, QString("Pstoedit"), message.str().c_str());
	} else {
		QMessageBox::information(this, QString("Pstoedit"), QString("Your version is up to date."));
	}
}

void PstoeditQtGui::CheckForUpdates() {
	auto status = connect(&HTTPmanager, &QNetworkAccessManager::finished,
						  this, &PstoeditQtGui::ReplyFinished);
	if (Verbose()) {
		cout << "Connection status:" << status << endl;
	}
	HTTPmanager.get(QNetworkRequest(QUrl("https://wglunz.users.sourceforge.net/version.txt")));
};

void PstoeditQtGui::About() {
	std::stringstream message;
	message << "**About pstoedit**\n\n"
		       "Pstoedit version: ";
	message << get_pstoedit_version() << "  \n" ;
	message <<	"Copyright (C) 1993 - 2024 Wolfgang Glunz " << "  \n"
				"All rights reserved" << "  \n"
				"Pstoedit home page:  [www.pstoedit.com](http://www.pstoedit.com \"pstoedit home page\")" << "  \n"
				"Refer to license.txt for conditions of distribution and use.\n\n"
				"This binary uses Qt version " << qVersion() << "  \n"
				"See Help->About Qt for more information about Qt." << endl;
	QMessageBox aboutBox(this);
	aboutBox.setWindowTitle("About pstoedit");
	aboutBox.setTextFormat(Qt::MarkdownText);
	aboutBox.setText(message.str().c_str());
	aboutBox.setIconPixmap(this->windowIcon().pixmap(100));
	aboutBox.exec();
};

void PstoeditQtGui::SupportPstoeditDevelopmentandMaintenance() const {
	QDesktopServices::openUrl(QUrl("https://wglunz.users.sourceforge.net/pstoedit_donate.htm", QUrl::StrictMode));
};

void PstoeditQtGui::Get_Support_or_open_a_Ticket() const {
	// QDesktopServices::openUrl(QUrl("https://pstoedit.sourceforge.io", QUrl::StrictMode));
	QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/pstoedit/", QUrl::StrictMode));
};

