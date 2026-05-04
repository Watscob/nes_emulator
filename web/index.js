Module = {
    onRuntimeInitialized: function() {
        Module.canvas = document.getElementById('canvas');
        const rect = Module.canvas.getBoundingClientRect();
        Module.canvas.width = Module.canvas.clientWidth;
        Module.canvas.height = Module.canvas.clientHeight;
        initEmulator(Module.canvas.width, Module.canvas.height);
    }
}

async function loadRomFromFile() {
    const fileInput = document.getElementById('rom-file');
    const file = fileInput.files[0];
    if (!file) {
        alert("Please select a ROM file!");
        return false;
    }

    const buffer = await file.arrayBuffer();
    const romData = new Uint8Array(buffer);

    try {
        FS.writeFile("/rom.nes", romData);

        if (!Module.load_rom_from_file("/rom.nes")) {
            console.error("Failed to load ROM");
            return false;
        }
    } catch (err) {
        console.error("Failed to load ROM (", err, ")");
        return false;
    }

    console.log("ROM loaded");
    return true;
}

function initEmulator(width, height) {
    try {
        Module.init_emulator(width, height);
    } catch (err) {
        console.error("An error occured while initializing the emulator (", err, ")");
    }
}

function startEmulator() {
    try {
        Module.start_emulator();
    } catch (err) {
        console.error("An error occured while starting the emulator (", err, ")");
    }
}

function stopEmulator() {
    try {
        Module.stop_emulator();
    } catch (err) {
        console.error("An error occured while stopping the emulator (", err, ")");
    }
}

function runEmulator() {
    loadRomFromFile().then((result) => {
        if (result) {
            startEmulator();
        }
    });
}

const romFileInput = document.getElementById('rom-file');
const fileLabel = document.getElementById('file-label');
romFileInput.addEventListener('change', function(event) {
  if (this.files.length > 0) {
    const fileName = this.files[0].name;
    fileLabel.textContent = fileName;
  } else {
    fileLabel.textContent = 'Choose ROM';
  }
});
