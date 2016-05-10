#include "ofxBezierWarpManager.h"



void ofxBezierWarpManager::loadShaders()
{
	m_edgeBlendShader.load("shaders/blend.vert", "shaders/blend.frag");
}

//--------------------------------------------------------------
ofxBezierWarpManager::ofxBezierWarpManager() {
	bBezierGuide = false;
	m_selectedId = -1;
}

//--------------------------------------------------------------
void ofxBezierWarpManager::setup(int _resolution) {
	warpResolution = _resolution;
	loadShaders();
}

//--------------------------------------------------------------
void ofxBezierWarpManager::setWarpResolution(int _resolution) {
	warpResolution = _resolution;
}

//--------------------------------------------------------------
void ofxBezierWarpManager::update() {
	// update bezier
	for (int i = 0; i < m_beziers.size(); i++) {
		m_beziers[i]->update();
	}
}

//--------------------------------------------------------------
void ofxBezierWarpManager::draw() {
	// draw bezier
	for (int i = 0; i < m_beziers.size(); i++) {
		m_beziers[i]->draw();
	}
}

//--------------------------------------------------------------
void ofxBezierWarpManager::keyPressed(int key)
{
	switch (key)
	{
	case 'g':
	case 'G': toggleGuideVisible(); return;
	
	case ',':
	case '<':selectPrev(); return;

	case '.':
	case '>':selectNext(); return;

	case 'b':
	case 'B':toggleBlend(); return;
	
	}

	if (m_selectedId == -1)return;
	m_beziers[m_selectedId]->keyPressed(key);

	//for (int i = 0; i < m_beziers.size(); i++) {
	//	m_beziers[i]->keyPressed(key);
	//}

}

//--------------------------------------------------------------
void ofxBezierWarpManager::keyReleased(int key) {
	if (m_selectedId == -1)return;
}


//--------------------------------------------------------------
void ofxBezierWarpManager::mouseDragged(int x, int y, int button) {
	if (m_selectedId == -1)return;
	m_beziers[m_selectedId]->mouseDragged(x, y, button);
	
	//for (int i = 0; i < m_beziers.size(); i++) {
	//	m_beziers[i]->mouseDragged(x, y, button);
	//}

}

//--------------------------------------------------------------
void ofxBezierWarpManager::mousePressed(int x, int y, int button) {
	if (m_selectedId == -1)return;
	m_beziers[m_selectedId]->mousePressed(x, y, button);
	
	//for (int i = 0; i < m_beziers.size(); i++) {
	//	m_beziers[i]->mousePressed(x, y, button);
	//}
}

//--------------------------------------------------------------
//void ofxBezierWarpManager::addFbo(ofFbo* _fbo){
//    cout << "[ofxBezierWarpManager] addFbo(ofFbo* _fbo)" << endl;
//    ofxBezierWarp _bezier;
//    
//    _bezier.setup(_fbo);
//    _bezier.setWarpResolution(warpResolution);
//    _bezier.setShader(&m_edgeBlendShader);
//    _bezier.setGridVisible(bBezierGuide);
//    m_beziers.push_back(_bezier);
//}

void ofxBezierWarpManager::createWarper(int w, int h)
{
	ofPtr<ofxBezierWarp> _bezier = ofPtr<ofxBezierWarp>(new ofxBezierWarp());

	_bezier->setup(m_beziers.size(),w, h, m_screenFboPtr);
	_bezier->setWarpResolution(warpResolution);
	_bezier->setShader(&m_edgeBlendShader);
	_bezier->setGridVisible(bBezierGuide);

	m_beziers.push_back(_bezier);

	__selectWarper(m_beziers.size() - 1);
}
//--------------------------------------------------------------
void ofxBezierWarpManager::removeFbo() {
	m_beziers.pop_back();
}

//--------------------------------------------------------------
void ofxBezierWarpManager::clear() {
	m_beziers.clear();
	// maskList.clear();
}

//--------------------------------------------------------------
void ofxBezierWarpManager::saveSettings() {
	//screen settings
	ofxXmlSettings _xml = ofxXmlSettings();
	int lastTagNumber;
	// screen
	for (int m = 0; m < m_beziers.size(); m++) {
		lastTagNumber = _xml.addTag("SCREEN");
		_xml.setValue("SCREEN:IS_BEZIER", (int)m_beziers[m]->anchorControl, lastTagNumber);
		_xml.setValue("SCREEN:RESOLUTION", (int)m_beziers[m]->prev_gridRes, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_EXPONENT", m_beziers[m]->edgeBlendExponent, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_LEFT", m_beziers[m]->edgeBlendAmountLeft, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_RIGHT", m_beziers[m]->edgeBlendAmountRight, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_TOP", m_beziers[m]->edgeBlendAmountTop, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_BOTTOM", m_beziers[m]->edgeBlendAmountBottom, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_GAMMA", m_beziers[m]->edgeBlendGamma, lastTagNumber);
		_xml.setValue("SCREEN:BLEND_LUMINANCE", m_beziers[m]->edgeBlendLuminance, lastTagNumber);
		_xml.setValue("SCREEN:SC_POS_X", m_beziers[m]->screen_pos_x, lastTagNumber);
		_xml.setValue("SCREEN:SC_POS_Y", m_beziers[m]->screen_pos_y, lastTagNumber);
		_xml.setValue("SCREEN:SC_SCALE_X", m_beziers[m]->screen_scale_x, lastTagNumber);
		_xml.setValue("SCREEN:SC_SCALE_Y", m_beziers[m]->screen_scale_y, lastTagNumber);
		_xml.setValue("SCREEN:ID", m_beziers[m]->getId(), lastTagNumber);

		if (_xml.pushTag("SCREEN", lastTagNumber)) {
			for (int c = 0; c < 4; c++) {
				int tagNum = _xml.addTag("CORNER");
				_xml.setValue("CORNER:X", (int)m_beziers[m]->corners[c].x, tagNum);
				_xml.setValue("CORNER:Y", (int)m_beziers[m]->corners[c].y, tagNum);
			}
			for (int a = 0; a < 8; a++) {
				int tagNum = _xml.addTag("ANCHOR");
				_xml.setValue("ANCHOR:X", (int)m_beziers[m]->anchors[a].x, tagNum);
				_xml.setValue("ANCHOR:Y", (int)m_beziers[m]->anchors[a].y, tagNum);
			}
			_xml.popTag();
		}
	}

	_xml.saveFile("BezierWarpManager_settings.xml");
}

//--------------------------------------------------------------
bool ofxBezierWarpManager::loadSettings() {
	ofxXmlSettings _xml;

	if (_xml.loadFile("BezierWarpManager_settings.xml")) {
		ofLogNotice() << "[BezierWarpManager_settings.xml] loaded!";
	}
	else {
		ofLogNotice() << "unable to load [BezierWarpManager_settings.xml] check data/ folder";
		return false;
	}

	int numMovieTags = _xml.getNumTags("SCREEN");

	// screen
	for (int n = m_beziers.size(); n < numMovieTags; n++)
	{
		// add bezier
		//ofPtr<ofxBezierWarp> _bezier = ofPtr<ofxBezierWarp>(new ofxBezierWarp());
		//m_beziers.push_back(_bezier);
		createWarper(1080, 1920);
	}

	for (int m = 0; m < numMovieTags; m++) {
		_xml.pushTag("SCREEN", m);
		ofLog(OF_LOG_NOTICE, " SCREEN:" + ofToString(m));

		int id = _xml.getValue("ID", m);
		m_beziers[id]->setId(id);

		m_beziers[id]->edgeBlendExponent = _xml.getValue("BLEND_EXPONENT", 1.0f);
		m_beziers[id]->edgeBlendAmountLeft = _xml.getValue("BLEND_LEFT", 0.0f);
		m_beziers[id]->edgeBlendAmountRight = _xml.getValue("BLEND_RIGHT", 0.0f);
		m_beziers[id]->edgeBlendAmountTop = _xml.getValue("BLEND_TOP", 0.0f);
		m_beziers[id]->edgeBlendAmountBottom = _xml.getValue("BLEND_BOTTOM", 0.0f);
		m_beziers[id]->edgeBlendGamma = _xml.getValue("BLEND_GAMMA", 1.8f);
		m_beziers[id]->edgeBlendLuminance = _xml.getValue("BLEND_LUMINANCE", 0.0f);
		m_beziers[id]->screen_pos_x = _xml.getValue("SC_POS_X", 0.0f);
		m_beziers[id]->screen_pos_y = _xml.getValue("SC_POS_Y", 0.0f);
		m_beziers[id]->screen_scale_x = _xml.getValue("SC_SCALE_X", 1.0f);
		m_beziers[id]->screen_scale_y = _xml.getValue("SC_SCALE_Y", 1.0f);


		m_beziers[id]->anchorControl = _xml.getValue("IS_BEZIER", 0);
		m_beziers[id]->gridRes = _xml.getValue("RESOLUTION", 10);
		m_beziers[id]->prev_gridRes = m_beziers[id]->gridRes;
		ofLog(OF_LOG_NOTICE, " IS_BEZIER:" + ofToString(m_beziers[id]->anchorControl));
		ofLog(OF_LOG_NOTICE, " RESOLUTION:" + ofToString(m_beziers[id]->prev_gridRes));
		

		int numCornerTags = _xml.getNumTags("CORNER");
		for (int i = 0; i < numCornerTags; i++) {
			int x = _xml.getValue("CORNER:X", 0, i);
			int y = _xml.getValue("CORNER:Y", 0, i);
			m_beziers[id]->corners[i].x = x;
			m_beziers[id]->corners[i].y = y;
			ofLog(OF_LOG_NOTICE, " CORNER:" + ofToString(i) + " x:" + ofToString(x) + " y:" + ofToString(y));
		}
		int numAnchorTags = _xml.getNumTags("ANCHOR");
		for (int i = 0; i < numAnchorTags; i++) {
			int x = _xml.getValue("ANCHOR:X", 0, i);
			int y = _xml.getValue("ANCHOR:Y", 0, i);
			m_beziers[id]->anchors[i].x = x;
			m_beziers[id]->anchors[i].y = y;
			ofLog(OF_LOG_NOTICE, " ANCHOR:" + ofToString(i) + " x:" + ofToString(x) + " y:" + ofToString(y));
		}
		_xml.popTag();
	}

	return true;
}

//--------------------------------------------------------------
void ofxBezierWarpManager::setGuideVisible(bool _visible, int _bezierNum) {
	if (_bezierNum <= -1) {
		bBezierGuide = _visible;
		for (int i = 0; i < m_beziers.size(); i++) {
			m_beziers[i]->setGridVisible(bBezierGuide);
		}
	}
	else {
		for (int i = 0; i < m_beziers.size(); i++) {
			if (i == _bezierNum) m_beziers[i]->setGridVisible(_visible);
		}
	}
}

//--------------------------------------------------------------
void ofxBezierWarpManager::toggleGuideVisible(int _bezierNum) {

	if (_bezierNum <= -1) {
		bBezierGuide = !bBezierGuide;
		for (int i = 0; i < m_beziers.size(); i++) {
			m_beziers[i]->setGridVisible(bBezierGuide);
		}
	}
	else {
		for (int i = 0; i < m_beziers.size(); i++) {
			if (i == _bezierNum) m_beziers[i]->setGridVisible(!m_beziers[i]->showGrid);
		}
	}
}
