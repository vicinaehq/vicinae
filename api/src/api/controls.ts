import { bus } from "./bus";

export const closeMainWindow = async () => {
  await bus.turboRequest("ui.closeMainWindow", {});
};

export const clearSearchBar = async () => {
  await bus.turboRequest("ui.setSearchText", { text: "" });
};

export const getSelectedText = async () => {
  const response = await bus.turboRequest("ui.getSelectedText", {});

  if (!response.ok) {
    throw new Error(`Failed to get selected text`);
  }

  return response.value.text;
};
