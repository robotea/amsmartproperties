/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   AMController.h
 * Author: zdenek
 *
 * Created on 3. září 2019, 17:09
 */

#ifndef AMCONTROLLER_H
#define AMCONTROLLER_H

#include "AMView.h"
#include "AMUICommon.h"
#include <memory>

class AMController
{
    std::shared_ptr<AMView> m_view;

public:
    AMController():m_view(nullptr) {AMProperty_log("AMController(%p)::AMController()\n",this);};
    ~AMController() {AMProperty_log("AMController(%p)::~AMController()\n",this);};
    void setView(std::shared_ptr<AMView> view) {AMProperty_log("AMController(%p)::setView(%p)\n",this,view.get());m_view=view;}
};

#endif /* AMCONTROLLER_H */

