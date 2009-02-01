/***************************************************************************
 *   Copyright (C) 2008 by Daniel Mack   *
 *   daniel@caiaq.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "MainWindow.h"
#include "WikiDisplay.h"

#include <QVBoxLayout>

void
MainWindow::powerButtonEvent(void)
{
}

void
MainWindow::searchButtonEvent(void)
{
}

void
MainWindow::treeButtonEvent(void)
{
}

void
MainWindow::randomButtonEvent(void)
{
}

MainWindow::MainWindow(QWidget *parent)
 : QWidget(parent)
{
    display = new WikiDisplay();
    powerSwitch = new QPushButton("&power/lid switch");
    batteryState = new QSlider(Qt::Horizontal);
    search = new QPushButton("Search");
    tree = new QPushButton("Tree");
    random = new QPushButton("Random"); 

    powerSwitch->setCheckable(TRUE);
    powerSwitch->setChecked(TRUE);
    connect(powerSwitch, SIGNAL(clicked()), this, SLOT(powerButtonEvent()));
    connect(search, SIGNAL(clicked()), this, SLOT(searchButtonEvent()));
    connect(tree, SIGNAL(clicked()), this, SLOT(treeButtonEvent()));
    connect(random, SIGNAL(clicked()), this, SLOT(randomButtonEvent()));

    batteryState->setValue(100);

    /*
     * Display and three button side by side
     * below the three hardware buttons
     */
    QHBoxLayout *buttonBox = new QHBoxLayout;
    buttonBox->addItem(new QSpacerItem(1,1,
                          QSizePolicy::MinimumExpanding,
                          QSizePolicy::MinimumExpanding));
    buttonBox->addWidget(search);
    buttonBox->addWidget(tree);
    buttonBox->addWidget(random);

    QVBoxLayout *controlSwitches = new QVBoxLayout;
    controlSwitches->addWidget(powerSwitch);
    controlSwitches->addWidget(batteryState);
    controlSwitches->addItem(new QSpacerItem(1,1,
                               QSizePolicy::MinimumExpanding,
                               QSizePolicy::MinimumExpanding));

    QVBoxLayout *displayBox = new QVBoxLayout;
    displayBox->addWidget(display);
    displayBox->addItem(buttonBox);
    displayBox->addItem(new QSpacerItem(1,1,
                          QSizePolicy::MinimumExpanding,
                          QSizePolicy::MinimumExpanding));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addItem(displayBox);
    mainLayout->addItem(controlSwitches);
    mainLayout->addItem(new QSpacerItem(4,4,
                          QSizePolicy::MinimumExpanding,
                          QSizePolicy::MinimumExpanding));
    setLayout(mainLayout);
}

MainWindow::~MainWindow()
{
	delete display;
}
