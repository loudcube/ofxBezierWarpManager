/*
 *  ofxBezierWarp.h
 *
 *  Created by Patrick Saint-Denis on 12-03-05.
 *	A Bezier Warp made of multiple homographies
 *  
 *  Forked by Teruaki Tsubokura on 13-08-28.
 *
 */

#pragma once

#include "ofMain.h"

class ofxBezierWarp{

public:
	ofxBezierWarp() {
		no = 0;
		layer = 0;
		gridRes = 0;
		prev_gridRes = 0;
		screen_pos_x = 0;
		screen_pos_y = 0.0f;
		screen_scale_x = 1.0f;
		screen_scale_y = 1.0f;
	}
	
    //void setup(ofFbo* _fbo);
	//void setup();
	//void setup(int _width, int _height);
	void setup(int _width, int _height, ofPtr<ofFbo> screenFboPtr, int grid = 10, int _layer = 0);
	
	void update(); // if you need
	void draw();
    void setShader(ofShader* edgeBlendShader)
    {
        m_edgeBlendShader = edgeBlendShader;
        edgeBlendExponent = 1.0;
        edgeBlendGamma = 1.8;
        edgeBlendLuminance = 0.0;
        edgeBlendAmountLeft = 0.2f;
        edgeBlendAmountRight = 0.2f;
        edgeBlendAmountTop = 0.0;
        edgeBlendAmountBottom = 0.0;
    }
	
    void resetAnchors();
	void save();
	void load();
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void keyPressed(int clef);
    bool isSelected();
    
    void setCanvasSize(int _width, int _height);
    void setWarpResolution(int _res);
    void setGridVisible(bool _visible);
    
//    bool bGradient;
    
	////////////
	float edgeBlendExponent;
    float edgeBlendAmountLeft;
    float edgeBlendAmountRight;
    float edgeBlendAmountTop;
    float edgeBlendAmountBottom;
    float edgeBlendGamma;
    float edgeBlendLuminance;
	////////////
	//screen fbo position,scale
	float screen_pos_x;
	float screen_pos_y;
	float screen_scale_x;
	float screen_scale_y;

	///////////

	///////////
	ofPoint corners[4];
	ofPoint anchors[8];
	int anchorControl;
	int prev_gridRes;
	int gridRes;
	bool showGrid;



private:
	int no;
	int layer;
	ofFbo m_srcFbo;
	ofFbo m_resultFbo;

	ofPtr<ofFbo> m_screenFboPtr;

	ofPoint center;
	
	int mouseON, spritesON;
	int selectedSprite[4], selectedControlPoint[8], selectedCenter;
	

	int mousePosX, mousePosY, rad;
	float width, height;

    void draw(ofTexture texture);
    void defaults();
	void sprites();
	float bezierPoint(float x0, float x1, float x2, float x3, float t);
    void drawGrid(float _stepX, float _stepY);
    
    ofShader* m_edgeBlendShader;
};