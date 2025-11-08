export default function ControlHandle() {
    return (
        <div
            className="flex flex-col items-center"
        >
            {/** Knob */}
            <button className="size-8 bg-[#7e0606] rounded-full hover:bg-[#9b1c1c] active:bg-[#5c0404] cursor-pointer" />
            {/** Stick */}
            <div className="w-1 bg-gray-600 h-10 rounded-md" />
        </div>
    )
}