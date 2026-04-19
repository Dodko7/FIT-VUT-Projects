/**
 * Function with a loading spinner. Amazing.
 */
export default function LudoLoadingPage() {
    return (
        <div className="fixed inset-0 flex items-center justify-center bg-gradient-to-b from-[#2e026d] to-[#15162c] text-white w-screen h-screen">
            <div className="loader ease-linear rounded-full border-8 border-t-8 border-gray-200 h-64 w-64"></div>
        </div>
    )
}