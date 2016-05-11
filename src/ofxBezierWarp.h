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


struct Widget
{
public:
	ofPoint* pos;
	int* selected;
};

class ofxBezierWarp{

public:
	ofxBezierWarp()
	{
		m_id = 0;
		layer = 0;
		gridRes = 0;
		prev_gridRes = 0;
		screen_pos_x = 0;
		screen_pos_y = 0.0f;
		screen_scale_x = 1.0f;
		screen_scale_y = 1.0f;

		m_bSelected = false;
		m_bBlend = true;

		int j = 0;
		for (int i = 0; i != 4; ++i)
		{
			m_selectChain[i * 3].selected = &(selectedSprite[i]);
			m_selectChain[i * 3].pos = &(corners[i]);
			m_selectChain[i * 3 + 1].selected = &(selectedControlPoint[j]);
			m_selectChain[i * 3 + 1].pos = &(anchors[j]);
			++j;
			m_selectChain[i * 3 + 2].selected = &(selectedControlPoint[j]);
			m_selectChain[i * 3 + 2].pos = &(anchors[j]);
			++j;
		}
	
	}
	
    //void setup(ofFbo* _fbo);
	//void setup();
	//void setup(int _width, int _height);
	void setup(int id, int _width, int _height, ofPtr<ofFbo> screenFboPtr, int grid = 10);
	
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
	
	int getId() { return m_id; }
	void setId(int id) { m_id = id; }


    void resetAnchors();
	void save();
	void load();
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void keyPressed(int clef);
    
	void select() { m_bSelected = true; }
	void deselect() { m_bSelected = false;  }
	bool isSelected();
    
    void setCanvasSize(int _width, int _height);
    void setWarpResolution(int _res);
    void setGridVisible(bool _visible);

	void toggleBlend()
	{
		m_bBlend = !m_bBlend;
	}

	void selectNextWidget()
	{
		if (anchorControl)
		{
			for (int i = 0; i != 12; ++i)
			{
				if (*(m_selectChain[i].selected) == 1)
				{
					*(m_selectChain[i].selected) = 0;
					if (i == 11)
					{
						*(m_selectChain[0].selected) = 1;
						mousePosX = m_selectChain[0].pos->x;
						mousePosY = m_selectChain[0].pos->y;
					}
					else
					{
						*(m_selectChain[i + 1].selected) = 1;
						mousePosX = m_selectChain[i + 1].pos->x;
						mousePosY = m_selectChain[i + 1].pos->y;
					}
					return;
				}
				
			}
		}
		else
		{
			for (int i = 0; i != 4; ++i)
			{
				if (selectedSprite[i] == 1)
				{
					selectedSprite[i] = 0;
					if (i == 3)
					{
						selectedSprite[0] = 1;
						mousePosX = corners[0].x;
						mousePosY = corners[0].y;
					}
					else
					{
						selectedSprite[i + 1] = 1;
						mousePosX = corners[i + 1].x;
						mousePosY = corners[i + 1].y;
					}
					return;
				}
			}
		}

		selectedSprite[0] = 1;
		mousePosX = corners[0].x;
		mousePosY = corners[0].y;
	}

	void selectPrevWidget()
	{
		if (anchorControl)
		{
			for (int i = 0; i != 12; ++i)
			{
				if (*(m_selectChain[i].selected) == 1)
				{
					*(m_selectChain[i].selected) = 0;
					if (i == 0)
					{
						*(m_selectChain[11].selected) = 1;
						mousePosX = m_selectChain[11].pos->x;
						mousePosY = m_selectChain[11].pos->y;
					}
					else
					{
						*(m_selectChain[i - 1].selected) = 1;
						mousePosX = m_selectChain[i - 1].pos->x;
						mousePosY = m_selectChain[i - 1].pos->y;
					}
					return;
				}
			}
		}
		else
		{
			for (int i = 0; i != 4; ++i)
			{
				if (selectedSprite[i] == 1)
				{
					selectedSprite[i] = 0;
					if (i == 0)
					{
						selectedSprite[3] = 1;
						mousePosX = corners[3].x;
						mousePosY = corners[3].y;
					}
					else
					{
						selectedSprite[i - 1] = 1;
						mousePosX = corners[i - 1].x;
						mousePosY = corners[i - 1].y;
					}
					return;
				}
			}
		}

		selectedSprite[0] = 1;
		mousePosX = corners[0].x;
		mousePosY = corners[0].y;
	}


    
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
	int m_id;
	int layer;
	ofFbo m_srcFbo;
	ofFbo m_resultFbo;

	ofPtr<ofFbo> m_screenFboPtr;

	ofPoint center;
	
	bool m_bSelected;
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

	bool m_bBlend;

	Widget m_selectChain[12];
};