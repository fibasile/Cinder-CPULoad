#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "CPUMonitor.h"
#include <stdio.h>
#include "cinder/Text.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include <boost/format.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

#define REFRESH_INTERVAL 1

class CPULoadApp : public AppBasic {
    CPUMonitor m_monitor;
    double m_lastUpdate;
    float m_lastLoad;
    Timer m_timer;
    gl::Texture m_background, m_topLabel, m_loadLabel, m_barFilled, m_barNormal;
    Font m_topLabelFont;
    Font m_loadLabelFont;

  public:
   	void prepareSettings(Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void CPULoadApp::setup()
{
    m_timer = Timer(true);
    m_monitor =  CPUMonitor();
     // setup gui
    m_lastUpdate = m_timer.getSeconds();
    m_lastLoad = 0;
 
    // load the background
    m_background = loadImage( loadResource( "background.jpg") );
    // load the normal and filled bars
    m_barNormal = loadImage( loadResource("bar_normal.jpg") );
    m_barFilled = loadImage( loadResource("bar_filled.jpg") );
    
    
    // setup the top label
    
    m_topLabelFont= Font( loadResource( "libelsuit.ttf" ), 24  );
    
    TextLayout simple;
    simple.setFont( m_topLabelFont );
    simple.setColor( Color( 1, 1, 1 ) );
    simple.addCenteredLine( "CPU Load" );
    
    m_topLabel = gl::Texture( simple.render( true, true ) );
    
    // setup the load label font
    
    m_loadLabelFont = Font(loadResource("libelsuit.ttf"), 32);
    
    
    
}
void CPULoadApp::prepareSettings(Settings *settings) {
    settings->setWindowSize( 230, 400 );
    settings->setFrameRate( REFRESH_INTERVAL / 2.0 );
    settings->setResizable(false);
}

void CPULoadApp::mouseDown( MouseEvent event )
{
}

void CPULoadApp::update()
{
    double now = m_timer.getSeconds();

    if ( now - m_lastUpdate > REFRESH_INTERVAL) {
        m_monitor.poll();
        m_lastUpdate = now;
        m_lastLoad = MAX(0, m_monitor.getLoad());
        
        
        int percent = ( m_lastLoad * 100);
        TextLayout simple;
        
        simple.setFont( m_loadLabelFont );
        simple.setColor( Color( 1, 1, 1 ) );
        
        std::string formatted;
        
        formatted = str( boost::format("%1%%2%") % percent % "%");
        
        
        simple.addCenteredLine( formatted  );
         m_loadLabel = gl::Texture( simple.render( true, true ) );

    }
  
}

void CPULoadApp::draw()
{
    
    double topMargin = 5;
    double leftMargin = 5;
    double rightMargin = 5;
    double bottomMargin = 5;
    double windowWidth = getWindowBounds().getWidth();
    double barWidth = m_barFilled.getWidth();
    double barHeight = m_barFilled.getHeight();
    double topPadding = 15;
    
    
    leftMargin = rightMargin = (windowWidth - barWidth) / 2.0;
    
    
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::setMatricesWindow( getWindowSize() );
    gl::enableAlphaBlending( true );
    

    gl::draw( m_background, getWindowBounds() );

    
    double bStart = topPadding;

    gl::draw( m_topLabel, Vec2f( (windowWidth - m_topLabel.getWidth() ) /2.0, bStart ));

    bStart += topMargin + barHeight + bottomMargin;

    for (int i=10; i>0; i--){
        
        Rectf shadowRect(leftMargin,bStart,leftMargin+barWidth-1,bStart+barHeight+1);

        gl::color( Color::black() );
        gl::drawSolidRect(shadowRect);

        gl::color( Color::white() );
        
        
        if ( m_lastLoad + 0.1  >= i/10.0f)  {
            gl::draw(m_barFilled, Vec2f( leftMargin, bStart));

        } else {
            gl::draw(m_barNormal, Vec2f( leftMargin, bStart));
        }
        bStart += topMargin + barHeight;

        
    }
    bStart += topMargin;
    if (m_loadLabel){
        gl::draw( m_loadLabel, Vec2f( (windowWidth - m_loadLabel.getWidth() ) /2.0, bStart ));
    }
    
}

CINDER_APP_BASIC( CPULoadApp, RendererGl )
