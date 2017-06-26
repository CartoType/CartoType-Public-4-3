//
//  CartoTypeDemoView.swift
//  CartoTypeSwiftDemo
//
//  Created by Graham Asher on 21/11/2016.
//  Copyright © 2016 Graham Asher. All rights reserved.
//

import UIKit

class CartoTypeDemoView : UIView
{
    var m_offset : CGPoint = CGPoint(x:0, y:0)
    var m_scale : CGFloat = 1
    var m_rotation : CGFloat = 0
    var m_current_point : CGPoint = CGPoint(x:0, y:0)
    var m_map_image : CGImage?
    
    override init(frame aFrame: CGRect)
    {
        super.init(frame: aFrame)
    }
    
    required init?(coder aDecoder: NSCoder)
    {
        super.init(coder: aDecoder)
    }
    
    override func draw(_ rect: CGRect)
    {
        let c = UIGraphicsGetCurrentContext()
        let h = bounds.size.height
        
        // Apply the transform representing current interactive panning, rotation and zooming.
        if (m_scale != 1 || m_rotation != 0)
        {
            c?.translateBy(x: m_current_point.x,y: m_current_point.y)
            c?.scaleBy(x: m_scale,y: m_scale)
            c?.rotate(by: m_rotation)
            c?.translateBy(x: -m_current_point.x,y: -m_current_point.y)
        }
        c?.translateBy(x: m_offset.x,y: m_offset.y)
        
        // Reflect the coordinate system vertically.
        c?.scaleBy(x: 1,y: -1)
        c?.translateBy(x: 0,y: -h)
        
        if (m_map_image != nil)
        {
            c?.draw(m_map_image!, in: rect)
        }
    }
}


