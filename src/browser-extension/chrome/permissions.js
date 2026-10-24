const CLOSE_DELAY_MS = 1500;

const PERMISSIONS = {
	bookmarks: {
		title: "Search your bookmarks from Vicinae",
		description: "Vicinae needs access to your bookmarks to list and search them. They are only shared with Vicinae on this computer.",
	},
	history: {
		title: "Search your history from Vicinae",
		description: "Vicinae needs access to your browsing history to search it and delete entries you choose. It is only shared with Vicinae on this computer, and nothing is deleted without your confirmation in Vicinae.",
	},
};

const titleEl = document.getElementById("title");
const descriptionEl = document.getElementById("description");
const allowBtn = document.getElementById("allowBtn");

const permission = new URLSearchParams(location.search).get("permission");
const info = PERMISSIONS[permission];

function setDescription(text, state = "") {
	descriptionEl.textContent = text;
	descriptionEl.className = `description ${state}`;
}

function showGranted() {
	allowBtn.hidden = true;
	setDescription("Access granted, you can go back to Vicinae.", "success");
	setTimeout(() => chrome.tabs.getCurrent((tab) => chrome.tabs.remove(tab.id)), CLOSE_DELAY_MS);
}

if (!info) {
	titleEl.textContent = "Unknown permission";
	setDescription(`Vicinae requested an unknown permission: ${permission}`, "error");
} else {
	titleEl.textContent = info.title;
	setDescription(info.description);

	chrome.permissions.contains({ permissions: [permission] }, (granted) => {
		if (granted) {
			showGranted();
			return;
		}
		allowBtn.hidden = false;
	});

	allowBtn.addEventListener("click", () => {
		chrome.permissions.request({ permissions: [permission] }, (granted) => {
			if (granted) {
				showGranted();
			} else {
				setDescription("Access was not granted. You can try again at any time.", "error");
			}
		});
	});
}
