#include "HttpServer.h"
#include "../httplib.h"
#include "KioskInterface.h"
#include "CentralRegistry.h"
#include "EventBus.h"
#include "FailureHandler.h"
#include "CSVManager.h"
#include "Subscribers.h"
#include <iostream>
#include <sstream>

std::string extractJsonString(const std::string& json, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if(pos == std::string::npos) return "";
    pos = json.find(":", pos);
    size_t start = json.find("\"", pos) + 1;
    size_t end = json.find("\"", start);
    return json.substr(start, end - start);
}

int extractJsonInt(const std::string& json, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if(pos == std::string::npos) return 0;
    pos = json.find(":", pos) + 1;
    while(pos < json.size() && (json[pos] == ' ' || json[pos] == '\"')) pos++;
    size_t end = pos;
    while(end < json.size() && isdigit(json[end])) end++;
    if (end == pos) return 0;
    return std::stoi(json.substr(pos, end - pos));
}

void AuraHttpServer::startServer() {
    auto maintenance = std::make_shared<MaintenanceService>();
    auto supply = std::make_shared<SupplyChainSystem>();
    auto city = std::make_shared<CityMonitoringCenter>();
    EventBus::getInstance().subscribe("HARDWARE_FAILURE", maintenance);
    EventBus::getInstance().subscribe("HARDWARE_FAILURE", city);
    EventBus::getInstance().subscribe("LOW_STOCK", supply);
    EventBus::getInstance().subscribe("EMERGENCY_ACTIVATED", city);

    httplib::Server svr;
    KioskInterface kiosk;

    // Premium UI Design
    const std::string html = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Aura Retail OS | Autonomous Control Center</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600&family=JetBrains+Mono:wght@400;500&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-dark: #0a0b10;
            --panel-bg: rgba(26, 28, 35, 0.8);
            --accent-blue: #3b82f6;
            --accent-red: #ef4444;
            --accent-green: #10b981;
            --accent-orange: #f59e0b;
            --text-main: #f3f4f6;
            --text-dim: #9ca3af;
            --glass-border: rgba(255, 255, 255, 0.1);
        }

        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Outfit', sans-serif; 
            background: var(--bg-dark); 
            color: var(--text-main);
            min-height: 100vh;
            display: flex;
            overflow-y: auto;
        }


        /* Sidebar */
        .sidebar {
            width: 280px;
            background: rgba(15, 17, 23, 0.95);
            border-right: 1px solid var(--glass-border);
            padding: 30px 20px;
            display: flex;
            flex-direction: column;
            gap: 40px;
        }

        .logo {
            font-size: 24px;
            font-weight: 600;
            background: linear-gradient(135deg, #fff 0%, var(--accent-blue) 100%);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            display: flex;
            align-items: center;
            gap: 10px;
        }

        .status-badge {
            padding: 15px;
            background: rgba(59, 130, 246, 0.1);
            border: 1px solid rgba(59, 130, 246, 0.2);
            border-radius: 12px;
        }

        .status-label { font-size: 12px; color: var(--text-dim); text-transform: uppercase; letter-spacing: 1px; }
        .status-value { font-size: 18px; margin-top: 5px; color: var(--accent-blue); font-weight: 600; }

        /* Main Content */
        .main-content {
            flex: 1;
            padding: 30px;
            overflow-y: auto;
            background: radial-gradient(circle at 50% 0%, rgba(59, 130, 246, 0.05) 0%, transparent 50%);
        }

        .grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 25px;
        }

        .panel {
            background: var(--panel-bg);
            backdrop-filter: blur(10px);
            border: 1px solid var(--glass-border);
            border-radius: 20px;
            padding: 25px;
            transition: transform 0.3s ease;
        }

        .panel:hover { border-color: rgba(59, 130, 246, 0.3); }

        h2 { font-size: 20px; margin-bottom: 20px; display: flex; align-items: center; gap: 10px; }

        /* Tables */
        .table-container { 
            width: 100%; 
            max-height: 300px; 
            overflow-y: auto; 
            scrollbar-width: thin;
        }

        table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 14px; }
        th { text-align: left; padding: 12px; color: var(--text-dim); font-weight: 400; border-bottom: 1px solid var(--glass-border); }
        td { padding: 12px; border-bottom: 1px solid rgba(255, 255, 255, 0.05); }

        /* Form & Buttons */
        .action-group { display: flex; flex-direction: column; gap: 15px; }
        .input-row { display: grid; grid-template-columns: 1fr 1fr 80px; gap: 10px; }
        input { 
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid var(--glass-border);
            padding: 12px;
            border-radius: 10px;
            color: white;
            font-family: inherit;
        }
        input:focus { outline: 2px solid var(--accent-blue); border-color: transparent; }

        button {
            padding: 12px 20px;
            border-radius: 10px;
            border: none;
            cursor: pointer;
            font-weight: 600;
            transition: all 0.2s;
            font-family: inherit;
            display: flex;
            align-items: center;
            justify-content: center;
            gap: 8px;
        }

        .btn-primary { background: var(--accent-blue); color: white; }
        .btn-primary:hover { background: #2563eb; transform: translateY(-2px); }

        .btn-danger { background: rgba(239, 68, 68, 0.1); color: var(--accent-red); border: 1px solid rgba(239, 68, 68, 0.2); }
        .btn-danger:hover { background: var(--accent-red); color: white; }

        .btn-warning { background: rgba(245, 158, 11, 0.1); color: var(--accent-orange); border: 1px solid rgba(245, 158, 11, 0.2); }
        .btn-warning:hover { background: var(--accent-orange); color: white; }

        .btn-ghost { background: transparent; color: var(--text-dim); border: 1px solid var(--glass-border); }
        .btn-ghost:hover { border-color: var(--text-main); color: var(--text-main); }

        /* Workflow Visualizer */
        .workflow-visualizer {
            grid-column: span 2;
            background: linear-gradient(90deg, rgba(59, 130, 246, 0.05), rgba(16, 185, 129, 0.05));
        }

        .steps { display: flex; justify-content: space-between; position: relative; margin-top: 20px; }
        .steps::before { content: ''; position: absolute; top: 15px; left: 0; right: 0; height: 2px; background: var(--glass-border); z-index: 0; }
        .step { position: relative; z-index: 1; background: var(--bg-dark); padding: 8px 18px; border-radius: 20px; border: 1px solid var(--glass-border); font-size: 12px; color: var(--text-dim); transition: all 0.4s ease; }
        .step.active { border-color: var(--accent-blue); color: #fff; box-shadow: 0 0 20px rgba(59, 130, 246, 0.4); transform: scale(1.1); z-index: 2; }
        .step.completed { border-color: var(--accent-green); color: var(--accent-green); opacity: 0.8; }

        /* Animations */
        @keyframes fadeIn { from { opacity: 0; transform: translateY(10px); } to { opacity: 1; transform: translateY(0); } }
        .panel { animation: fadeIn 0.5s ease out forwards; }

        /* Custom Scrollbar */
        ::-webkit-scrollbar { width: 10px; }
        ::-webkit-scrollbar-track { background: rgba(0, 0, 0, 0.2); }
        ::-webkit-scrollbar-thumb { 
            background: rgba(59, 130, 246, 0.5); 
            border-radius: 10px; 
            border: 2px solid var(--bg-dark);
        }
        ::-webkit-scrollbar-thumb:hover { background: var(--accent-blue); }

        /* Select styling fix */
        select option {
            background: #1a1c23;
            color: white;
            padding: 10px;
        }
    </style>

    <script>
        let currentWorkflow = "IDLE";
        
        async function fetchStatus() {
            let res = await fetch('/status');
            let data = await res.json();
            document.getElementById('status').innerText = data.status;
            document.getElementById('mode').innerText = data.mode;
            
            // Update visual state based on mode
            const badge = document.querySelector('.status-badge');
            if (data.mode === 'EMERGENCY') {
                badge.style.borderColor = 'var(--accent-red)';
                badge.style.background = 'rgba(239, 68, 68, 0.1)';
            } else if (data.mode === 'DISCOUNT') {
                badge.style.borderColor = 'var(--accent-green)';
                badge.style.background = 'rgba(16, 185, 129, 0.1)';
            } else {
                badge.style.borderColor = 'rgba(59, 130, 246, 0.2)';
                badge.style.background = 'rgba(59, 130, 246, 0.1)';
            }
        }

        async function fetchInventory() {
            let res = await fetch('/inventory');
            let data = await res.json();
            
            const productSelect = document.getElementById('productId');
            const currentSelected = productSelect.value;
            
            let html = '<table><tr><th>ID</th><th>Product Name</th><th>Stock</th><th>Price</th></tr>';
            let selectHtml = '<option value="">Select Product...</option>';
            data.forEach(item => {
                let stockColor = item.available_stock < 5 ? 'var(--accent-red)' : 'inherit';
                html += `<tr>
                    <td><code style="color:var(--accent-blue)">${item.id}</code></td>
                    <td>${item.name}</td>
                    <td style="color:${stockColor}">${item.available_stock}</td>
                    <td style="font-weight:600">₹${item.final_price.toFixed(2)}</td>
                </tr>`;
                selectHtml += `<option value="${item.id}">${item.id} - ${item.name}</option>`;
            });
            html += '</table>';
            document.getElementById('inv').innerHTML = html;
            productSelect.innerHTML = selectHtml;
            
            // Restore selection
            if (currentSelected) {
                productSelect.value = currentSelected;
            }
        }


        async function resetInventory() {
            await fetch('/simulate/reset_inventory', { method: 'POST' });
            refreshAll();
            alert("Inventory Reset to Default State!");
        }

        async function fetchEvents() {
            let res = await fetch('/events');
            let data = await res.json();
            let html = '<table><tr><th>Time</th><th>Severity</th><th>Message</th></tr>';
            data.reverse().forEach(ev => {
                let color = 'var(--text-dim)';
                if(ev.severity === 'CRITICAL') color = 'var(--accent-red)';
                if(ev.severity === 'WARNING') color = 'var(--accent-orange)';
                
                let date = new Date(parseInt(ev.timestamp) * 1000);
                let timeStr = date.toLocaleTimeString([], {hour: '2-digit', minute:'2-digit', second:'2-digit', hour12: false});
                
                html += `<tr>
                    <td style="font-family:'JetBrains Mono'; font-size:12px">${timeStr}</td>
                    <td><span style="color:${color}; font-size:11px; font-weight:600">[${ev.severity}]</span></td>
                    <td>${ev.message}</td>
                </tr>`;
            });
            html += '</table>';
            document.getElementById('evt').innerHTML = html;
        }

        async function fetchTransactions() {
            let res = await fetch('/transactions');
            let data = await res.json();
            let html = '<table><tr><th>Txn ID</th><th>Product</th><th>Qty</th><th>Final Price</th><th>Status</th></tr>';
            data.reverse().forEach(tx => {
                let statusBg = tx.status === 'SUCCESS' ? 'rgba(16, 185, 129, 0.1)' : 'rgba(239, 68, 68, 0.1)';
                let statusColor = tx.status === 'SUCCESS' ? 'var(--accent-green)' : 'var(--accent-red)';

                html += `<tr>
                    <td><code style="font-size:11px">${tx.txn_id}</code></td>
                    <td>${tx.product_id}</td>
                    <td>${tx.qty}</td>
                    <td>₹${tx.final_price.toFixed(2)}</td>
                    <td><span style="padding:4px 8px; border-radius:6px; font-size:10px; background:${statusBg}; color:${statusColor}">${tx.status}</span></td>
                </tr>`;
            });
            html += '</table>';
            document.getElementById('txn').innerHTML = html;
        }

        async function doPost(url, body, workflowName) {
            // Start the actual backend request in parallel with animation
            const backendPromise = fetch(url, { method: 'POST', body: JSON.stringify(body) });
            
            // Trigger the step-by-step animation
            await animateWorkflow(workflowName);
            
            let res = await backendPromise;
            let data = await res.json();
            
            if (workflowName === 'PURCHASE') {
                if (data.success) {
                    updateWorkflowUI(workflowName, 5, true); // Final success state
                    setTimeout(() => alert(data.message), 100);
                } else {
                    updateWorkflowUI(workflowName, 5, false); // Final failed state
                    setTimeout(() => alert("Error: " + data.message), 100);
                }
            } else {
                updateWorkflowUI(workflowName, 5, true);
            }
            
            refreshAll();
        }

        async function animateWorkflow(type) {
            const stepsCount = 5;
            for (let i = 0; i < stepsCount; i++) {
                updateWorkflowUI(type, i, null);
                await new Promise(r => setTimeout(r, 400)); // Time spent in each phase
            }
        }

        function updateWorkflowUI(type, currentIndex, finalSuccess) {
            const container = document.getElementById('workflow-steps');
            let steps = [];
            if (type === 'PURCHASE') {
                steps = ['Request', 'Check Inventory', 'Apply Strategy', 'Execute Command', 'Commit CSV'];
            } else if (type === 'EMERGENCY') {
                steps = ['Trigger', 'Switch State', 'Publish Event', 'Notify Subscribers', 'Lockdown'];
            } else if (type === 'FAILURE') {
                steps = ['Detect Error', 'Retry Chain', 'Recalibrate', 'Alert Tech', 'Log Failure'];
            } else if (type === 'MODE_CHANGE') {
                steps = ['UI Event', 'Update Context', 'Change Strategy', 'Notify Bus', 'Sync State'];
            } else {
                steps = ['Idle', 'Waiting...', '...', '...', '...'];
            }

            let html = '';
            steps.forEach((step, i) => {
                let statusClass = '';
                if (i < currentIndex) statusClass = 'completed';
                else if (i === currentIndex) statusClass = 'active';
                
                // If it's the last step and we have a final result
                if (currentIndex === 5) {
                    statusClass = (finalSuccess === false && i === 4) ? 'active' : 'completed';
                }

                html += `<div class="step ${statusClass}">${step}</div>`;
            });
            container.innerHTML = html;
            
            let statusText = currentIndex < 5 ? 'RUNNING' : (finalSuccess === false ? 'FAILED' : 'COMPLETED');
            document.getElementById('workflow-title').innerText = `Active Workflow: ${type} (${statusText})`;
        }

        function refreshAll() { fetchStatus(); fetchInventory(); fetchEvents(); fetchTransactions(); }
        setInterval(refreshAll, 5000);
        window.onload = () => {
            refreshAll();
            updateWorkflowUI('IDLE', 'NONE');
        };
    </script>
</head>
<body>
    <div class="sidebar">
        <div class="logo">
            <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2L2 7l10 5 10-5-10-5zM2 17l10 5 10-5M2 12l10 5 10-5"/></svg>
            Aura OS
        </div>
        
        <div class="status-badge">
            <div class="status-label">System Status</div>
            <div id="status" class="status-value">Connecting...</div>
        </div>

        <div class="status-badge" style="border-color: rgba(255,255,255,0.1); background: transparent;">
            <div class="status-label">Operation Mode</div>
            <div id="mode" class="status-value" style="color: var(--text-main)">Standard</div>
        </div>

        <div style="margin-top: auto; font-size: 12px; color: var(--text-dim);">
            Path A: Adaptive Autonomous<br>
            v1.0.4-stable
        </div>
    </div>

    <div class="main-content">
        <div class="grid">
            <!-- Workflow Visualizer -->
            <div class="panel workflow-visualizer">
                <h2 id="workflow-title">System Workflow Visualizer</h2>
                <div class="steps" id="workflow-steps">
                    <!-- Dynamic Steps -->
                </div>
                <p style="margin-top: 15px; font-size: 13px; color: var(--text-dim);">
                    This panel visualizes the internal logic flow (Design Patterns) in real-time.
                </p>
            </div>

            <!-- Inventory Panel -->
            <div class="panel">
                <h2>
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 8V21H3V8M1 3H23V8H1V3ZM10 12H14"/></svg>
                    Real-time Inventory
                </h2>
                <div class="table-container" id="inv"></div>
            </div>

            <!-- Control Panel -->
            <div class="panel">
                <h2>
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
                    Action Command Center
                </h2>
                <div class="action-group">
                    <div class="input-row" style="grid-template-columns: 80px 1fr 80px;">
                        <input id="userId" placeholder="User ID" value="U123" style="background: rgba(0, 0, 0, 0.3); border: 1px solid var(--glass-border); padding: 12px; border-radius: 10px; color: white; font-family: inherit;">
                        <select id="productId" style="background: rgba(0, 0, 0, 0.3); border: 1px solid var(--glass-border); padding: 12px; border-radius: 10px; color: white; font-family: inherit;">
                            <option>Loading products...</option>
                        </select>
                        <input id="qty" type="number" placeholder="Qty" value="1">
                    </div>
                    <button class="btn-primary" onclick="doPost('/purchase', {userId: document.getElementById('userId').value, productId: document.getElementById('productId').value, qty: parseInt(document.getElementById('qty').value)}, 'PURCHASE')">
                        Process Purchase Transaction
                    </button>
                    
                    <div style="height: 1px; background: var(--glass-border); margin: 10px 0;"></div>
                    
                    <button class="btn-primary" style="background: var(--glass-bg); border: 1px solid var(--accent-orange); color: var(--accent-orange);" onclick="resetInventory()">
                        Reset/Restock Inventory
                    </button>


                    
                    <div style="height: 1px; background: var(--glass-border); margin: 10px 0;"></div>
                    
                    <div style="display: grid; grid-template-columns: 1fr 1fr; gap: 10px;">
                        <button class="btn-warning" onclick="doPost('/simulate/failure', {}, 'FAILURE')">Simulate HW Failure</button>
                        <button class="btn-danger" onclick="doPost('/simulate/emergency', {}, 'EMERGENCY')">Trigger Emergency</button>
                    </div>

                    <div style="display: flex; gap: 10px;">
                        <button class="btn-ghost" style="flex:1" onclick="doPost('/mode', {mode: 'ACTIVE'}, 'MODE_CHANGE')">ACTIVE</button>
                        <button class="btn-ghost" style="flex:1" onclick="doPost('/mode', {mode: 'DISCOUNT'}, 'MODE_CHANGE')">DISCOUNT</button>
                        <button class="btn-ghost" style="flex:1" onclick="doPost('/mode', {mode: 'POWER_SAVING'}, 'MODE_CHANGE')">ECO</button>
                    </div>
                </div>
            </div>

            <!-- Events Panel -->
            <div class="panel">
                <h2>
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z"/><polyline points="14 2 14 8 20 8"/><line x1="16" y1="13" x2="8" y2="13"/><line x1="16" y1="17" x2="8" y2="17"/><polyline points="10 9 9 9 8 9"/></svg>
                    System Event Logs
                </h2>
                <div class="table-container" id="evt"></div>
            </div>

            <!-- Transactions Panel -->
            <div class="panel">
                <h2>
                    <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="5" width="20" height="14" rx="2"/><line x1="2" y1="10" x2="22" y2="10"/></svg>
                    Recent Transactions
                </h2>
                <div class="table-container" id="txn"></div>
            </div>
        </div>
    </div>
</body>
</html>
    )html";


    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        res.set_content(html, "text/html");
    });

    svr.Get("/status", [&](const httplib::Request&, httplib::Response& res) {
        std::string mode = CentralRegistry::getInstance().getMode();
        std::string status = kiosk.getStatus();
        std::string json = "{\"status\": \"" + status + "\", \"mode\": \"" + mode + "\"}";
        res.set_content(json, "application/json");
    });

    svr.Get("/inventory", [&](const httplib::Request&, httplib::Response& res) {
        auto products = kiosk.getInventory()->getAllProducts();
        std::string json = "[";
        for(size_t i=0; i<products.size(); ++i) {
            int avail = kiosk.getInventory()->getAvailableStock(products[i].product_id);
            double finalPrice = kiosk.getPricingStrategy()->computePrice(products[i].price, 1);
            json += "{\"id\": \"" + products[i].product_id + "\", \"name\": \"" + products[i].name + 
                    "\", \"available_stock\": " + std::to_string(avail) + ", \"final_price\": " + std::to_string(finalPrice) + "}";
            if(i < products.size()-1) json += ",";
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Get("/events", [&](const httplib::Request&, httplib::Response& res) {
        auto events = CSVManager::getInstance().readEvents();
        std::string json = "[";
        for(size_t i=0; i<events.size(); ++i) {
            json += "{\"timestamp\": \"" + events[i]["timestamp"] + "\", \"type\": \"" + events[i]["type"] + 
                    "\", \"severity\": \"" + events[i]["severity"] + "\", \"message\": \"" + events[i]["message"] + "\"}";
            if(i < events.size()-1) json += ",";
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Get("/transactions", [&](const httplib::Request&, httplib::Response& res) {
        auto txns = CSVManager::getInstance().readTransactions();
        std::string json = "[";
        for(size_t i=0; i<txns.size(); ++i) {
            json += "{\"txn_id\": \"" + txns[i].txn_id + "\", \"product_id\": \"" + txns[i].product_id + 
                    "\", \"qty\": " + std::to_string(txns[i].qty) + ", \"final_price\": " + std::to_string(txns[i].final_price) + 
                    ", \"status\": \"" + txns[i].status + "\"}";
            if(i < txns.size()-1) json += ",";
        }
        json += "]";
        res.set_content(json, "application/json");
    });

    svr.Post("/purchase", [&](const httplib::Request& req, httplib::Response& res) {
        std::string userId = extractJsonString(req.body, "userId");
        std::string productId = extractJsonString(req.body, "productId");
        int qty = extractJsonInt(req.body, "qty");
        auto result = kiosk.purchaseItem(userId, productId, qty);
        res.set_content("{\"success\": " + std::string(result.success ? "true" : "false") + ", \"message\": \"" + result.message + "\"}", "application/json");
    });


    svr.Post("/mode", [&](const httplib::Request& req, httplib::Response& res) {
        std::string mode = extractJsonString(req.body, "mode");
        kiosk.switchMode(mode);
        EventBus::getInstance().publish("MODE_CHANGED", "Switched to " + mode, "INFO");
        res.set_content("{\"success\": true}", "application/json");
    });

    svr.Post("/simulate/emergency", [&](const httplib::Request&, httplib::Response& res) {
        EventBus::getInstance().publish("EMERGENCY_ACTIVATED", "Emergency lockdown initiated!", "CRITICAL");
        kiosk.switchMode("EMERGENCY");
        res.set_content("{\"success\": true}", "application/json");
    });

    svr.Post("/simulate/reset_inventory", [&](const httplib::Request&, httplib::Response& res) {
        std::vector<ProductRecord> defaultInv = {
            {"101", "Paracetamol", "Medicine", 5.0, 10, 0, "none"},
            {"102", "Insulin", "Medicine", 25.0, 10, 0, "refrigeration"},
            {"103", "Bandages", "Medicine", 3.5, 100, 0, "none"},
            {"201", "Sandwich", "Food", 4.0, 50, 0, "refrigeration"},
            {"202", "Protein Bar", "Food", 2.5, 80, 0, "none"},
            {"203", "Water Bottle", "Food", 1.5, 85, 0, "none"},
            {"301", "First Aid Kit", "Emergency", 15.0, 25, 0, "none"},
            {"302", "Flashlight", "Emergency", 10.0, 40, 0, "none"},
            {"303", "Thermal Blanket", "Emergency", 8.0, 50, 0, "none"},
            {"304", "Emergency Radio", "Emergency", 20.0, 15, 0, "none"}
        };
        CSVManager::getInstance().writeInventory(defaultInv);
        EventBus::getInstance().publish("SYSTEM_RESET", "Inventory has been reset to default state.", "INFO");
        res.set_content("{\"success\": true}", "application/json");
    });

    svr.Post("/simulate/failure", [&](const httplib::Request&, httplib::Response& res) {
        EventBus::getInstance().publish("HARDWARE_FAILURE", "Sensor malfunction detected.", "WARNING");
        auto alert = std::make_shared<TechnicianAlertHandler>();
        auto recalibrate = std::make_shared<RecalibrationHandler>();
        auto retry = std::make_shared<AutoRetryHandler>();
        retry->setNext(recalibrate);
        recalibrate->setNext(alert);

        FailureContext ctx{"SYSTEM", "Sensor Error", false};
        retry->handle(ctx);

        res.set_content("{\"success\": true}", "application/json");
    });

    std::cout << "Starting Aura Retail OS on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}
