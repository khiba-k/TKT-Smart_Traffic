import { BrowserRouter, Routes, Route } from "react-router-dom";
import { ClerkProvider } from "@clerk/clerk-react";
import Home from "./pages/Home";
import Welcome from "./pages/Welcome"
import React from "react";

const PUBLISHABLE_KEY = import.meta.env.VITE_CLERK_PUBLISHABLE_KEY;

if (!PUBLISHABLE_KEY) {
  throw new Error("Missing Publishable Key");
}

export default function App() {
    return (
        <ClerkProvider publishableKey={PUBLISHABLE_KEY} afterSignOutUrl="/">
            <BrowserRouter>
                <Routes>
                    <Route path="/Main" element={<Home />} />
                    <Route path="/" element={<Welcome />} />
                </Routes>
            </BrowserRouter>
        </ClerkProvider>
    )
}