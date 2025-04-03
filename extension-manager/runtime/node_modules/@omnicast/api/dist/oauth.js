"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.PKCEClient = void 0;
class PKCEClient {
    redirectMethod;
    providerName;
    providerIcon;
    providerId;
    description;
    resolvesOnRedirect;
    isAuthorizing;
    constructor(options) {
        this.providerId = options.providerId;
        this.providerName = options.providerName;
        this.providerIcon = options.providerIcon;
        this.description = options.description;
        this.redirectMethod = options.redirectMethod;
        this.resolvesOnRedirect = false;
        this.isAuthorizing = false;
    }
    /**
     * Creates an authorization request for the provided authorization endpoint, client ID, and scopes.
     * You need to first create the authorization request before calling {@link OAuth.PKCEClient.authorize}.
     *
     * @remarks The generated code challenge for the PKCE request uses the S256 method.
     *
     * @returns A promise for an {@link OAuth.AuthorizationRequest} that you can use as input for {@link OAuth.PKCEClient.authorize}.
     */
    async authorizationRequest(options) {
        return {};
    }
    /**
     * Starts the authorization and shows the OAuth overlay in Raycast.
     * As parameter you can either directly use the returned request from {@link OAuth.PKCEClient.authorizationRequest},
     * or customize the URL by extracting parameters from {@link OAuth.AuthorizationRequest} and providing your own URL via {@link AuthorizationOptions}.
     * Eventually the URL will be used to open the authorization page of the provider in the web browser.
     *
     * @returns A promise for an {@link OAuth.AuthorizationResponse}, which contains the authorization code needed for the token exchange.
     * The promise is resolved when the user was redirected back from the provider's authorization page to the Raycast extension.
     */
    async authorize(options) {
        return {};
    }
    authorizationURL;
    /**
     * Securely stores a {@link OAuth.TokenSet} for the provider. Use this after fetching the access token from the provider.
     * If the provider returns a a standard OAuth JSON token response, you can directly pass the {@link OAuth.TokenResponse}.
     * At a minimum, you need to set the {@link OAuth.TokenSet.accessToken}, and typically you also set {@link OAuth.TokenSet.refreshToken} and {@link OAuth.TokenSet.isExpired}.
     * Raycast automatically shows a logout preference for the extension when a token set was saved.
     *
     * @remarks If you want to make use of the convenience {@link OAuth.TokenSet.isExpired} method, the property {@link OAuth.TokenSet.expiresIn} must be configured.
     *
     * @returns A promise that resolves when the token set has been stored.
     */
    async setTokens(options) {
    }
    /**
     * Retrieves the stored {@link OAuth.TokenSet} for the client.
     * You can use this to initially check whether the authorization flow should be initiated or
     * the user is already logged in and you might have to refresh the access token.
     *
     * @returns A promise that resolves when the token set has been retrieved.
     */
    async getTokens() {
        return undefined;
    }
    /**
     * Removes the stored {@link OAuth.TokenSet} for the client.
     *
     * @remarks Raycast automatically shows a logout preference that removes the token set.
     * Use this method only if you need to provide an additional logout option in your extension or you want to remove the token set because of a migration.
     *
     */
    async removeTokens() { }
}
exports.PKCEClient = PKCEClient;
