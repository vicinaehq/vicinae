import { LocalStorage } from '@vicinae/api';

// We support deprecated local storage methods:
// https://developers.raycast.com/misc/migration/v1.28.0#storage

export const allLocalStorageItems = LocalStorage.allItems;
export const getLocalStorageItem = LocalStorage.getItem;
export const setLocalStorageItem = LocalStorage.setItem;
export const removeLocalStorageItem = LocalStorage.removeItem;
export const clearLocalStorage = LocalStorage.clear;

export type LocalStorageValues = LocalStorage.Values;
export type LocalStorageValue = LocalStorage.Value;

export { LocalStorage } from '@vicinae/api';
