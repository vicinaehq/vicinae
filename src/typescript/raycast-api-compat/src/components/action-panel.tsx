import * as vicinae from '@vicinae/api';

// older versions of the Raycast API had ActionPanel.Item
export const ActionPanel = Object.assign(vicinae.ActionPanel, {
	Item: vicinae.Action
})
