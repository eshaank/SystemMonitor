//
//  SystemMonitor.swift
//  SystemMonitor
//
//  Created by Eshaan Kansagara on 8/10/24.
//
import SwiftUI

@main
struct SystemMonitor: App {
        @State var currentNumber: String = "1"
        @State private var cpuUsage: Double = 0.0
        @State private var usedMemory: Double = 0.0
        @State private var totalMemory: Double = 0.0
    
        // State variable to track updates
        @State private var timerTrigger = Date()
    
        var body: some Scene {
            MenuBarExtra(currentNumber, systemImage: "\(currentNumber).circle") {
                Text("CPU Usage: \(cpuUsage, specifier: "%.2f")%")
                Text("Used RAM: \(usedMemory, specifier: "%.2f") GB")
                Text("Total RAM: \(totalMemory, specifier: "%.2f") GB")
    
                Divider()
    
                Button(action: updateSystemUsage) {
                    Text("Update System Usage")
                }
            }
            .onChange(of: timerTrigger) { _ in
                updateSystemUsage()
            }
        }
    
        func updateSystemUsage() {
            cpuUsage = get_cpu_usage()
            get_ram_usage(&usedMemory, &totalMemory)
        }
    
        func startTimer() {
            Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { _ in
                timerTrigger = Date()
            }
        }
}
    


