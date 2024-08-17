//
//  SystemMonitor.swift
//  SystemMonitor
//
//  Created by Eshaan Kansagara on 8/10/24.
//
import SwiftUI

@main
struct SystemMonitor: App {
    var body: some Scene {
        MenuBarExtra("UtilityApp", systemImage: "hammer") {
            AppMenu()
        }.menuBarExtraStyle(.window)
    }
}

struct AppMenu: View {
    @State private var cpuUsage: Double = 0
    @State private var memoryUsage: Double = 0
    @State private var diskUsage: Double = 0
    
    @State private var timer: Timer? = nil
    
    var body: some View {
        VStack {
            HStack(spacing: 16) {
                ProgressView(progress: $cpuUsage)
                ProgressView(progress: $memoryUsage)
                ProgressView(progress: $diskUsage)
            }
            .padding()
        }
        .onAppear {
            startTimer()
        }
        .onDisappear {
            timer?.invalidate()
        }
    }
    
    func updateSystemUsage() {

        cpuUsage = get_cpu_usage()
        memoryUsage = get_memory_usage()
        diskUsage = get_disk_usage()
        
        print("CPU Usage: \(cpuUsage)%")
        print("Used Memory: \(memoryUsage) GB")
        print("Total Memory: \(diskUsage) GB")
    }
    
    func startTimer() {
        timer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { _ in
            updateSystemUsage()
        }
    }
}

struct ProgressView: View {
    @Binding var progress: Double

    var body: some View {
        VStack {
            ZStack {
                CircularProgressView(progress: progress / 100) // Convert percentage to percentage
                Text("\(progress, specifier: "%.0f")%")
                    .font(.title2)
                    .bold()
            }
            .frame(width: 60, height: 60)
        }
    }
}


struct CircularProgressView: View {
    let progress: Double
    
    var body: some View {
        ZStack {
            Circle()
                .stroke(
                    Color.pink.opacity(0.5),
                    lineWidth: 5
                )
            Circle()
                .trim(from: 0, to: progress)
                .stroke(
                    Color.pink,
                    style: StrokeStyle(
                        lineWidth: 5,
                        lineCap: .round
                    )
                )
                .rotationEffect(.degrees(-90))
                .animation(.easeOut, value: progress)
        }
    }
}
