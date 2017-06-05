//
//  ViewController.swift
//  CartoTypeSwiftDemo
//
//  Created by Graham Asher on 20/11/2016.
//  Copyright © 2016 CartoType Ltd. All rights reserved.
//

import UIKit

class ViewController: UIViewController, UIGestureRecognizerDelegate
{
    @IBOutlet weak var m_view: CartoTypeDemoView!
    
    // MARK: Properties
    var m_framework: CartoTypeFramework?
    var m_route_start = CartoTypePoint(x:0, y:0)
    var m_route_end = CartoTypePoint(x:0, y:0)
    var m_ui_scale: CGFloat = UIScreen.main.scale
    
    required init?(coder aDecoder: NSCoder)
    {
        super.init(coder: aDecoder)
    }
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        let view = m_view!
        
        // Create a pan gesture recognizer.
        let my_pan_recognizer = UIPanGestureRecognizer(target: self,action: #selector(ViewController.handlePanGesture(_:)))
        my_pan_recognizer.delegate = self
        view.addGestureRecognizer(my_pan_recognizer)
        
        // Create a pinch gesture recognizer.
        let my_pinch_recognizer = UIPinchGestureRecognizer(target: self,action: #selector(ViewController.handlePinchGesture(_:)))
        my_pinch_recognizer.delegate = self
        view.addGestureRecognizer(my_pinch_recognizer)
        
        // Create a rotation gesture recognizer.
        let my_rotation_recognizer = UIRotationGestureRecognizer(target: self,action: #selector(ViewController.handleRotationGesture(_:)))
        my_rotation_recognizer.delegate = self
        view.addGestureRecognizer(my_rotation_recognizer)
        
        // Create a tap gesture recognizer.
        let my_tap_recognizer = UITapGestureRecognizer(target: self,action: #selector(ViewController.handleTapGesture(_:)))
        my_tap_recognizer.delegate = self
        view.addGestureRecognizer(my_tap_recognizer)
        
        // Create the framework object.
        let width = view.frame.size.width * m_ui_scale
        let height = view.frame.size.height * m_ui_scale
        let param = CartoTypeFrameworkParam()
        param.mapFileName = "santa-cruz"
        param.styleSheetFileName = "standard"
        param.fontFileName = "DejaVuSans"
        param.viewWidth = Int32(width)
        param.viewHeight = Int32(height)
        m_framework = CartoTypeFramework(param: param)
        
        // Draw the first map.
        view.m_map_image = m_framework!.getMapBitmap().takeUnretainedValue()
        view.m_scale = 1
        view.setNeedsDisplay()
    }
    
    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func gestureRecognizer(_ gestureRecognizer: UIGestureRecognizer, shouldRecognizeSimultaneouslyWith otherGestureRecognizer: UIGestureRecognizer)->Bool
    {
        return true
    }
    
    func RadiansToDegrees(_ aRadians:Double)->Double { return aRadians / Double.pi * 180.0 }
    
    func setRouteEnd(_ aPoint: CGPoint)
    {
        m_route_start = m_route_end
        m_route_end.x = Double(aPoint.x * m_ui_scale)
        m_route_end.y = Double(aPoint.y * m_ui_scale)
        m_framework?.convert(&m_route_end, from: ScreenCoordType, to: DegreeCoordType)
        if (m_route_start.x != 0 && m_route_start.y != 0)
        {
            let error = m_framework?.startNavigation(from: m_route_start, start: DegreeCoordType, to: m_route_end, end: DegreeCoordType)
            if (error == 0)
            {
                m_view!.m_map_image = m_framework!.getMapBitmap().takeUnretainedValue()
                m_view!.setNeedsDisplay()
            }
        }
    }
    
    func acceptGesture()
    {
        let framework = m_framework!
        let view = m_view!
        
        framework.panX(Int32(-view.m_offset.x * m_ui_scale), andY: Int32(-view.m_offset.y * m_ui_scale))
        if (view.m_scale != 1 || view.m_rotation != 0)
        {
            let w = view.bounds.size.width
            let h = view.bounds.size.height
            let cx = view.bounds.origin.x + w / 2
            let cy = view.bounds.origin.y + h / 2
            let x = Int32((cx - view.m_current_point.x) * m_ui_scale)
            let y = Int32((cy - view.m_current_point.y) * m_ui_scale)
            framework.panX(-x, andY:-y)
            framework.zoom(Double(view.m_scale))
            framework.rotate(RadiansToDegrees(Double(view.m_rotation)))
            framework.panX(x, andY:y)
        }
        
        view.m_offset = CGPoint.zero
        view.m_scale = 1
        view.m_rotation = 0
        view.m_current_point = CGPoint.zero
        
        view.m_map_image = m_framework!.getMapBitmap().takeUnretainedValue()
        view.setNeedsDisplay()
    }
    
    func rejectGesture()
    {
        let view = m_view!
        view.m_offset = CGPoint.zero
        view.m_scale = 1
        view.m_rotation = 0
        view.m_current_point = CGPoint.zero
        view.setNeedsDisplay()
    }
    
    
    func handlePanGesture(_ aRecognizer:UIPanGestureRecognizer)
    {
        let view = m_view!
        if (aRecognizer.state == UIGestureRecognizerState.changed)
        {
            view.m_offset = aRecognizer.translation(in: nil)
            view.m_current_point = aRecognizer.location(in: nil)
            view.setNeedsDisplay()
        }
        else if (aRecognizer.state == UIGestureRecognizerState.recognized)
        {
            acceptGesture()
            aRecognizer.setTranslation(CGPoint.zero, in: nil)
        }
        else if (aRecognizer.state == UIGestureRecognizerState.cancelled)
        {
            rejectGesture()
        }
    }
    
    func handlePinchGesture(_ aRecognizer:UIPinchGestureRecognizer)
    {
        let view = m_view!
        if (aRecognizer.state == UIGestureRecognizerState.changed)
        {
            view.m_scale = aRecognizer.scale
            view.m_current_point = aRecognizer.location(in: nil)
            view.setNeedsDisplay()
        }
        else if (aRecognizer.state == UIGestureRecognizerState.recognized)
        {
            acceptGesture()
            aRecognizer.scale = 1
        }
        else if (aRecognizer.state == UIGestureRecognizerState.cancelled)
        {
            rejectGesture()
        }
    }
    
    func handleRotationGesture(_ aRecognizer:UIRotationGestureRecognizer)
    {
        let view = m_view!
        if (aRecognizer.state == UIGestureRecognizerState.changed)
        {
            view.m_rotation = aRecognizer.rotation
            view.m_current_point = aRecognizer.location(in: nil)
            view.setNeedsDisplay()
        }
        else if (aRecognizer.state == UIGestureRecognizerState.recognized)
        {
            acceptGesture()
            aRecognizer.rotation = 0
        }
        else if (aRecognizer.state == UIGestureRecognizerState.cancelled)
        {
            rejectGesture()
        }
    }
    
    func handleTapGesture(_ aRecognizer:UITapGestureRecognizer)
    {
        if (aRecognizer.state == UIGestureRecognizerState.recognized)
        {
            setRouteEnd(aRecognizer.location(in: nil))
        }
    }
}

