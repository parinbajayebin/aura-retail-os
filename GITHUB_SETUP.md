# GitHub Repository Setup Instructions

## Quick Setup

1. **Create a new repository on GitHub:**
   - Go to https://github.com/new
   - Repository name: `aura-retail-os`
   - Description: `Aura Retail OS - Smart Kiosk System (IT620 Project)`
   - Public/Private: Your choice
   - Click "Create repository"

2. **Upload this code to GitHub:**

```bash
cd aura-retail-os
git init
git add .
git commit -m "Subtask 2: Implemented Singleton, Strategy, and Command patterns"
git branch -M main
git remote add origin https://github.com/YOUR-USERNAME/aura-retail-os.git
git push -u origin main
```

3. **What to include in your submission:**
   - GitHub repository link
   - README.md (already included)
   - Source code (src/ folder)
   - Class diagram (CLASS_DIAGRAM.md)
   - This setup guide

## Repository Structure

After uploading, your repository will have:

```
aura-retail-os/
├── README.md                  # Project documentation
├── CLASS_DIAGRAM.md           # Updated class diagram
├── GITHUB_SETUP.md            # This file
├── run.sh                     # Compilation script
├── src/
│   ├── AuraKiosk.h           # All pattern implementations
│   ├── main.cpp              # Simulation
│   └── aura_kiosk            # Compiled binary
└── data/                      # For future CSV files
```

## Submission Checklist

For Subtask 2, ensure you have:

✅ Partial source code (minimal working implementation)
✅ At least 2 design patterns implemented (we have 3!)
   - Singleton Pattern ✓
   - Strategy Pattern ✓
   - Command Pattern ✓
✅ Basic working simulation (Dynamic Pricing scenario)
✅ Updated class diagram
✅ GitHub repository link
✅ README with clear documentation

## How to Share Your Repository

Share this link format with your professor:
```
https://github.com/YOUR-USERNAME/aura-retail-os
```

## Clone Instructions (for team members)

```bash
git clone https://github.com/YOUR-USERNAME/aura-retail-os.git
cd aura-retail-os
chmod +x run.sh
./run.sh
```

---

**Note:** Replace `YOUR-USERNAME` with your actual GitHub username
